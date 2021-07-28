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
Determine the min number of containers required for a given number of clients,
which alternate between having a stateful (= they prefer having a dedicated
container) vs. stateless nature (= they are OK with being assigned to a pool
of shared stateless containers), so that the system is stable and the
probability that a stateful container is assigned to a shared pool of
stateless containers is below a given threshold (epsilon).
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
#include <vector>

namespace po = boost::program_options;

double compute_C_F_max(const size_t C_k, const size_t N_k,
                       const double lambda_k, const double mu_L) {
  return C_k * (mu_L - lambda_k * N_k / C_k) / (mu_L - lambda_k);
}

double binom(const size_t n, const size_t k) {
  // prepare input
  std::vector<double> myVec1(k);
  std::vector<double> myVec2(k);
  for (size_t i = n - k + 1, j = 0; i <= n; i++, j++) {
    myVec1[j] = i;
  }
  for (size_t i = k, j = 0; i >= 1; i--, j++) {
    myVec2[j] = i;
  }

  // compute return value
  double ret = 1.0;
  for (size_t i = 0; i < k; i++) {
    ret *= myVec1[i] / myVec2[i];
  }
  return ret;
}

double P_0(const size_t N_k, const double q_L, const double q_F) {
  return std::pow(q_L / (q_F + q_L), N_k);
}

double P_i(const size_t N_k, const size_t i, const double q_L,
           const double q_F) {
  return P_0(N_k, q_L, q_F) * binom(N_k, i) * std::pow(q_F / q_L, i);
}

/**
 * \return the probability that a function requiring a dedicated container
 * is assigned instead to a pool of shared stateless containers.
 */
double compute_P_v(const size_t C_F_max, const size_t N_k, const double q_L,
                   const double q_F) {
  assert(C_F_max > 0);
  assert(N_k > 0);

  double ret = 0;

  VLOG(2) << "q_L = " << q_L << ", q_F = " << q_F << " N_k = " << N_k << ", P0 "
          << P_0(N_k, q_L, q_F);

  for (size_t i = C_F_max; i <= N_k; i++) {
    ret += P_i(N_k, i, q_L, q_F);
  }

  return ret / (1 - P_0(N_k, q_L, q_F));
}

int main(int argc, char *argv[]) {
  uiiit::support::GlogRaii myGlogRaii(argv[0]);

  size_t N_k; // number of clients
  double inv_mu_F;
  double inv_mu_L;
  double lambda_k;
  double q_F;
  double q_L;
  double epsilon;

#ifndef NDEBUG
  assert(std::abs(binom(10, 4)) - 210.0 < 0.1);
  assert(std::abs(binom(20, 10)) - 184756.0 < 0.1);
  assert(std::abs(binom(30, 10)) - 30045015.0 < 0.1);
#endif

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
     po::value<double>(&q_F)->default_value(20),
     "Transition rate of clients in a stateless state.")
    ("q-less",
     po::value<double>(&q_L)->default_value(80),
     "Transition rate of clients in a stateful state.")
    ("epsilon",
     po::value<double>(&epsilon)->default_value(0.01),
     "Maximum accepted probability that a client in stateful state is served by a shared container.")
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

    for (size_t C_k = 1; C_k <= N_k; C_k++) {
      // maximum number of containers that can be dedicated to stateful use
      // while allowing the stateless clients to remain stable
      // note: the number can be negative, in which case the system will not be
      // stable for stateless clients even though they are left with _all_ the
      /// containers
      auto C_F_max_real = compute_C_F_max(C_k, N_k, lambda_k, mu_L);
      auto C_F_max_int =
          static_cast<size_t>(C_F_max_real < 0 ? 0 : C_F_max_real);

      if (C_F_max_int == 0) {
        VLOG(1) << "C_F_max = " << C_F_max_real << ": system unstable";
        continue;
      }

      // probability that a stateful container "overflows" to the pool
      // of shared stateless containers
      auto P_v = compute_P_v(C_F_max_int, N_k, q_L, q_F);

      VLOG(1) << "mu_F = " << mu_F << ", mu_L = " << mu_L << ", C_k = " << C_k
              << ", C_F_max = " << C_F_max_real << ", P_v = " << P_v;

      // if the probability is below threshold, quit
      if (P_v <= epsilon) {
        std::cout << C_k << ' ' << (static_cast<double>(C_k) / N_k) << ' '
                  << (C_k / (N_k * q_F / (q_F + q_L))) << std::endl;
        break;
      }
    }

    return EXIT_SUCCESS;

  } catch (const std::exception &aErr) {
    LOG(ERROR) << "Exception caught: " << aErr.what();

  } catch (...) {
    LOG(ERROR) << "Unknown exception caught";
  }

  return EXIT_FAILURE;
}
