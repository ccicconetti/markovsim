#include "Markov/mmc.h"
#include "Support/chrono.h"
#include "Support/glograii.h"

#include <boost/program_options.hpp>

#include <glog/logging.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  uiiit::support::GlogRaii myGlogRaii(argv[0]);

  size_t       mySeed;
  double       lambda;
  double       mu;
  unsigned int c;
  double       myWarmup;
  double       myDuration;
  double       myThreshold;
  std::string  myDistFile;

  po::options_description myDesc("Allowed options");
  // clang-format off
  myDesc.add_options()
    ("help,h", "produce help message")
    ("seed",
     po::value<size_t>(&mySeed)->default_value(std::chrono::system_clock::now().time_since_epoch().count()),
     "Seed generator. If not specified use the number of seconds since Jan 1, 1970.")
    ("lambda",
     po::value<double>(&lambda)->default_value(1.0),
     "Arrival rate.")
    ("mu",
     po::value<double>(&mu)->default_value(1.0),
     "Serving rate.")
    ("c",
     po::value<unsigned int>(&c)->default_value(2),
     "Number of servers.")
    ("warmup",
     po::value<double>(&myWarmup)->default_value(30),
     "The warm-up duration, in s.")
    ("duration",
     po::value<double>(&myDuration)->default_value(60),
     "Simulation duration step, in s.")
    ("threshold",
     po::value<double>(&myThreshold)->default_value(0.001),
     "Convergence threshold, in s.")
    ("distributions",
     po::value<std::string>(&myDistFile)->default_value(""),
     "Write the full distributions on file with the given base name.")
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

    uiiit::markov::MMC     myMMC(lambda, mu, c, mySeed);
    const auto             myTheoDelay = myMMC.averageResponseTime();
    auto                   myMeasDelay = std::numeric_limits<double>::max();
    uiiit::support::Chrono myChrono(true);
    LOG(INFO) << "starting simulation";
    myMMC.run(myWarmup);
    myMMC.statEnable();
    auto mySimulTime = myWarmup;
    while (fabs(myTheoDelay - myMeasDelay) > myThreshold) {
      mySimulTime += myDuration;
      myMMC.run(myDuration);
      myMeasDelay = myMMC.averageDelay();
      VLOG(1) << "avg response time: theo " << myTheoDelay
              << " vs. meas: " << myMeasDelay;
    }
    LOG(INFO) << "done in " << mySimulTime << " s (" << myChrono.stop()
              << " s real time)";

    if (not myDistFile.empty()) {
      myMMC.sortDelays();
      size_t i = 0;
      for (const auto& myDeque : myMMC.delays()) {
        ++i;
        if (myDeque.empty()) {
          continue;
        }
        std::ofstream myOut(myDistFile +
                            std::to_string(i * 100 / myMMC.delays().size()) +
                            ".dat");
        for (const auto& myDelay : myDeque) {
          myOut << myDelay << '\n';
        }
      }
    }

    return EXIT_SUCCESS;

  } catch (const std::exception& aErr) {
    LOG(ERROR) << "Exception caught: " << aErr.what();

  } catch (...) {
    LOG(ERROR) << "Unknown exception caught";
  }

  return EXIT_FAILURE;
}
