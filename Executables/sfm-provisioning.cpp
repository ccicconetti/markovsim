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

/*
Compute the average latency of a pool of FaaS workers, where:
- one group is assigned one stateful container each;
- another group of workers share a pool of stateless containers.
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

int main(int argc, char *argv[]) {
  uiiit::support::GlogRaii myGlogRaii(argv[0]);

  size_t N_k; // number of clients
  double inv_mu_F;
  double inv_mu_L;
  double lambda_k;
  double q_F;
  double q_L;
  double epsilon;

  std::string myOutput;

  po::options_description myDesc("Allowed options");
  // clang-format off
  myDesc.add_options()
    ("help,h", "produce help message")
    ("arrival-rate",
     po::value<double>(&lambda_k)->default_value(0.075),
     "Arrival rate, in Hz.")
    ("clients",
     po::value<size_t>(&N_k)->default_value(70),
     "Number of clients")
    ("service-time-full",
     po::value<double>(&inv_mu_F)->default_value(1.0),
     "Service time for clients assigned a dedicated container, in s.")
    ("service-time-less",
     po::value<double>(&inv_mu_L)->default_value(3.0),
     "Service time for clients sharing a pool of non-dedicated containers, in s.")
    ("q-full",
     po::value<double>(&q_F)->default_value(80),
     "Transition rate of clients in a stateless state.")
    ("q-less",
     po::value<double>(&q_L)->default_value(20),
     "Transition rate of clients in a stateful state.")
    ("epsilon",
     po::value<double>(&epsilon)->default_value(0.01),
     "Maximum accepted probability that a client in stateful state is served by a shared container.")
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

    if (inv_mu_F <= 0) {
      throw std::runtime_error("Invalid service time (full): " +
                               std::to_string(inv_mu_F));
    }
    double mu_F = 1.0 / inv_mu_F;
    if (inv_mu_L <= 0) {
      throw std::runtime_error("Invalid service time (less): " +
                               std::to_string(inv_mu_L));
    }
    double mu_L = 1.0 / inv_mu_L;

    if (q_F <= 0) {
      throw std::runtime_error("Invalid transition rate (full): " +
                               std::to_string(q_F));
    }
    if (q_L <= 0) {
      throw std::runtime_error("Invalid transition rate (less): " +
                               std::to_string(q_L));
    }
    if (epsilon <= 0 or epsilon >= 1) {
      throw std::runtime_error("Invalid epsilon: " + std::to_string(epsilon));
    }

    std::ofstream myOutfile(myOutput);
    if (not myOutfile) {
      throw std::runtime_error("Could not open file: " + myOutput);
    }

    VLOG(1) << "mu_F = " << mu_F << ", mu_L = " << mu_L;

    return EXIT_SUCCESS;

  } catch (const std::exception &aErr) {
    LOG(ERROR) << "Exception caught: " << aErr.what();

  } catch (...) {
    LOG(ERROR) << "Unknown exception caught";
  }

  return EXIT_FAILURE;
}
