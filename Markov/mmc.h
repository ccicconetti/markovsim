/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      Tiny Markov simulator
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/markovsim/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2018 Claudio Cicconetti <https://about.me/ccicconetti>

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

#pragma once

#include "Support/macros.h"
#include "Support/stat.h"

#include <array>
#include <deque>
#include <list>
#include <memory>
#include <queue>
#include <random>
#include <utility>
#include <vector>

namespace uiiit {
namespace markov {

class MMC final {
  NONCOPYABLE_NONMOVABLE(MMC);

  using LoadList = std::list<std::pair<double, unsigned int>>;
  using DelayArray = std::array<std::deque<double>, 10>;

  //! A server in the simulation, processing a single customer at a time.
  struct Server {
    explicit Server() : theBusy(false), theLastArrival(0) {}

    bool theBusy;
    double theLastArrival;
  };

  //! Simulation event.
  struct Event {
    enum Type { ARRIVAL, DEPARTURE };

    //! Create an arrival event.
    explicit Event(const double aExpireTime) noexcept
        : theType(ARRIVAL), theServerIndex(0), theExpireTime(aExpireTime) {}

    //! Create a departure event.
    explicit Event(const double aExpireTime, const size_t aServerIndex) noexcept
        : theType(DEPARTURE), theServerIndex(aServerIndex),
          theExpireTime(aExpireTime) {}

    bool operator<(const Event &aOther) const noexcept {
      return theExpireTime > aOther.theExpireTime;
    }

    Type theType;
    size_t theServerIndex; // only if DEPARTURE
    double theExpireTime;
  };

public:
  //! Initialize a Marvov queue M/M/c model.
  explicit MMC(const double lambda_, const double mu_, const unsigned int c_,
               const size_t aSeedSeq);

  //! \return the steady-state average response time.
  double averageResponseTime() const;

  //! Run a simulation with given duration (possibly resuming).
  void run(const double aDuration);

  //! \return the average delay measured so far.
  double averageDelay() const { return theDelayStat.mean(); }

  //! Enable statistics collection.
  void statEnable() { theStatEnabled = true; }

  //! Sort the delays per load value per increasing delay.
  void sortDelays();

  //! \return the delays per load value.
  const DelayArray &delays() const { return theDelays; }

private:
  //! A new customer arrived.
  void arrival();

  //! A customer departed from a given server.
  void departure(const size_t aServerIndex);

private:
  // const ctor arguments
  const double lambda;
  const double mu;
  const unsigned int c;
  const double theLoadPeriod;

  // derived
  const double rho;
  std::vector<std::unique_ptr<std::seed_seq>> theSeedSeqs;
  std::vector<std::mt19937> theGenerators;
  std::vector<std::exponential_distribution<double>> theExpos;

  // internal state
  bool theStatEnabled;
  double theClock;
  std::vector<Server> theServers;
  size_t theBusyServers;
  std::list<double> theQueue;
  std::priority_queue<Event> theEvents;
  support::SummaryStat theDelayStat;
  DelayArray theDelays;
  LoadList theLoads;
};

} // namespace markov
} // namespace uiiit
