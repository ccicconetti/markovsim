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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>

namespace po = boost::program_options;

double erlang_c([[maybe_unused]] size_t aWorkers,
                [[maybe_unused]] double aLoad) {
  return 1.0;
}

int main(int argc, char *argv[]) {
  uiiit::support::GlogRaii myGlogRaii(argv[0]);

  size_t N_k = 40; // number of containers
  size_t C_k = 70; // number of clients
  double mu_F = 1.0;
  double mu_L = 1.0 / 3;
  double lambda_k = 0.075;

  po::options_description myDesc("Allowed options");
  // clang-format off
  myDesc.add_options()
    ("help,h", "produce help message")
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

      double L = (n_F * L_F + n_L * L_L) / N_k;
      std::cout << n_F << ' ' << L_L << ' ' << L << std::endl;
    }

    return EXIT_SUCCESS;

  } catch (const std::exception &aErr) {
    LOG(ERROR) << "Exception caught: " << aErr.what();

  } catch (...) {
    LOG(ERROR) << "Unknown exception caught";
  }

  return EXIT_FAILURE;
}
