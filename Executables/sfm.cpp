/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitous Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      Stateful FaaS Model Latency Simulator
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/markovsim/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2021 Claudio Cicconetti <https://ccicconetti.github.io/>

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Support/chrono.h"
#include "Support/glograii.h"

#include <boost/program_options.hpp>

#include <glog/logging.h>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>

namespace po = boost::program_options;

double erlang_c([[maybe_unused]] size_t aWorkers,
                [[maybe_unused]] double aLoad) {
  assert(aWorkers > 0);
  assert(aLoad > 0);

  //            A
  // return ---------
  //          A + B

  // compute A
  double myFact = 1.0; // (aWorkers-1) * (aWorkers-2) * ... * 2 * 1
  for (size_t i = 2; i < aWorkers; i++) {
    myFact *= i;
  }
  double A = std::pow(aLoad, aWorkers) / (myFact * (aWorkers - aLoad));

  // compute B
  double B = 0;
  double myCurFact = 1.0;
  for (size_t i = 0; i < aWorkers; i++) {
    if (i > 0) {
      myCurFact *= i;
    }
    B += std::pow(aLoad, i) / myCurFact;
  }

  return A / (A + B);
}

int main(int argc, char *argv[]) {
  uiiit::support::GlogRaii myGlogRaii(argv[0]);

  size_t N_k; // number of containers
  size_t C_k; // number of clients
  double mu_F;
  double mu_L;
  double lambda_k;

  std::string myOutput;

  po::options_description myDesc("Allowed options");
  // clang-format off
  myDesc.add_options()
    ("help,h", "produce help message")
    ("arrival-rate",
     po::value<double>(&lambda_k)->default_value(0.075),
     "Arrival rate, in Hz.")
    ("containers",
     po::value<size_t>(&N_k)->default_value(40),
     "Number of containers")
    ("clients",
     po::value<size_t>(&C_k)->default_value(70),
     "Number of clients")
    ("service-time-full",
     po::value<double>(&mu_F)->default_value(1.0),
     "Service time for clients assigned a dedicated container.")
    ("service-time-less",
     po::value<double>(&mu_L)->default_value(1.0 / 3),
     "Service time for clients sharing a pool of non-dedicated containers.")
    ("output",
     po::value<std::string>(&myOutput)->default_value("out.dat"),
     "Output file.")
    ;
  // clang-format on

  try {
    po::variables_map myVarMap;
    po::store(po::parse_command_line(argc, argv, myDesc), myVarMap);
    po::notify(myVarMap);

    if (myVarMap.count("help")) {
      std::cout << myDesc << std::endl;
      return EXIT_FAILURE;
    }

    std::ofstream myOutfile(myOutput);
    if (not myOutfile) {
      throw std::runtime_error("Could not open file: " + myOutput);
    }

    for (size_t n_F = 0; n_F < N_k; n_F++) {
      auto n_L = C_k - n_F;
      auto lambda_L = lambda_k * n_L;
      auto C_L = N_k - n_F;
      double L_F = 1.0 / (mu_F - lambda_k);
      auto rho_F = lambda_k / mu_F;
      auto rho_L = lambda_L / (mu_L * C_L);

      if (rho_F >= 1.0 or rho_L >= 1.0) {
        VLOG(1) << "rho_F = " << rho_F << ", rho_L = " << rho_L
                << ": system unstable";
        continue;
      }

      double L_L =
          erlang_c(C_L, lambda_L / mu_L) / (mu_L * C_L - lambda_L) + 1.0 / mu_L;

      double L = (n_F * L_F + n_L * L_L) / C_k;
      myOutfile << n_F << ' ' << L << '\n';
    }

    return EXIT_SUCCESS;

  } catch (const std::exception &aErr) {
    LOG(ERROR) << "Exception caught: " << aErr.what();

  } catch (...) {
    LOG(ERROR) << "Unknown exception caught";
  }

  return EXIT_FAILURE;
}
