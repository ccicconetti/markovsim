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

#include "mmc.h"

#include "Support/math.h"

#include <glog/logging.h>

#include <algorithm>
#include <cassert>
#include <cmath>

namespace uiiit {
namespace markov {

MMC::MMC(const double       lambda_,
         const double       mu_,
         const unsigned int c_,
         const size_t       aSeedSeq)
    : lambda(lambda_)
    , mu(mu_)
    , c(c_)
    , theLoadPeriod(30)
    , rho(lambda / (c * mu))
    , theSeedSeqs(1 + c)
    , theGenerators(1 + c)
    , theExpos(1 + c)
    , theStatEnabled(false)
    , theClock(0)
    , theServers(c)
    , theBusyServers(0)
    , theQueue()
    , theEvents()
    , theDelayStat()
    , theDelays()
    , theLoads() {
  if (lambda <= 0) {
    throw std::runtime_error("lambda cannot be <= 0");
  }
  if (mu <= 0) {
    throw std::runtime_error("mu cannot be <= 0");
  }
  if (c == 0) {
    throw std::runtime_error("c cannot be == 0");
  }
  LOG_IF(WARNING, rho >= 1) << "the system is unstable";

  // initialize the seed generators
  assert(theSeedSeqs.size() == theGenerators.size());
  assert(theSeedSeqs.size() == theExpos.size());
  for (size_t i = 0; i < theSeedSeqs.size(); i++) {
    theSeedSeqs[i].reset(new std::seed_seq({aSeedSeq, i}));
    theGenerators[i].seed(*theSeedSeqs[i]);

    if (i < c) {
      // departure exponential distributions
      theExpos[i].param(std::exponential_distribution<double>::param_type(mu));

    } else {
      assert(i == c);

      // arrival exponential distribution
      theExpos[i].param(
          std::exponential_distribution<double>::param_type(lambda));
    }
  }

  // create the first arrival event
  theEvents.push(Event(0));
}

double MMC::averageResponseTime() const {
  if (rho >= 1) {
    throw std::runtime_error("the system is unstable");
  }
  return support::math::erlangC(c, rho) / (c * mu - lambda) + 1.0 / mu;
}

void MMC::run(const double aDuration) {
  const auto myEndTime = theClock + aDuration;
  while (theClock <= myEndTime) {
    assert(not theEvents.empty());

    // retrieve the next event
    const auto& myEvent = theEvents.top();

    // advance the clock
    assert(myEvent.theExpireTime >= theClock);
    theClock = myEvent.theExpireTime;

    // perform actions depending on the event type
    switch (myEvent.theType) {
      case Event::ARRIVAL:
        arrival();
        break;
      case Event::DEPARTURE:
        departure(myEvent.theServerIndex);
        break;
      default:
        assert(false);
    }

    // remove the exhausted event
    theEvents.pop();
  }
}

void MMC::arrival() {
  assert(theGenerators.size() > c);
  assert(theExpos.size() > c);

  // dispatch the new customer to an available server or to the waiting queue
  if (theBusyServers < c) {
    // find the first available server (order does not matter)
    for (size_t i = 0; i < c; i++) {
      if (not theServers[i].theBusy) {
        // mark this server as busy
        theServers[i].theLastArrival = theClock;
        theServers[i].theBusy        = true;
        theBusyServers++;
        theLoads.push_back({theClock, theBusyServers});

        // add a new event for when this customer will leave from server
        const auto myDepartureTime = theClock + theExpos[i](theGenerators[i]);
        theEvents.emplace(Event(myDepartureTime, i));
        break;
      }
    }
  } else {
    // no available servers, the customer will have to wait
    theQueue.push_back(theClock);
  }

  // generate the event for the new arrival
  const auto myNextArrival = theClock + theExpos[c](theGenerators[c]);
  theEvents.emplace(Event(myNextArrival));
}

void MMC::departure(const size_t aServerIndex) {
  assert(aServerIndex < theServers.size());
  assert(theServers[aServerIndex].theBusy);
  assert(not theLoads.empty());

  //
  // compute the utilization (also remove stale values from theLoads)
  //

  // lower bound of the window to compute the load
  const auto myWindowStart = theClock - theLoadPeriod; // may be negative

  // remove all stale elements
  auto it = theLoads.begin();
  auto jt = it;
  ++jt;
  while (jt != theLoads.end() and jt->first < myWindowStart) {
    ++jt;
    it = theLoads.erase(it);
  }

  // two cases:
  // 1. jt == theLoads.end(): the list consists of a single element
  // 2. it != theLoads.end(): it points to the element before the window start
  //    and jt points to the element after the window start, unless the window
  //    starts < 0
  if (jt == theLoads.end()) {
    assert(theLoads.size() == 1);
  } else if (myWindowStart >= 0) {
    assert(it->first <= myWindowStart);
    assert(jt->first >= myWindowStart);
  }
  assert(not theLoads.empty());

  auto myAvgLoad = 0.0;
  auto myLast    = theLoads.front();
  auto kt        = theLoads.begin();
  myLast         = *kt;
  ++kt;
  for (; kt != theLoads.end(); ++kt) {
    const auto mySpan = kt->first - std::max(myWindowStart, myLast.first);
    myAvgLoad += mySpan * myLast.second;
    myLast = *kt;
  }
  const auto mySpan = theClock - std::max(myWindowStart, myLast.first);
  myAvgLoad += mySpan * myLast.second;

  const auto myTotSpan =
      theClock - std::max(myWindowStart, theLoads.front().first);
  const auto myNormLoad = myAvgLoad / myTotSpan / c;
  assert(myNormLoad >= 0.0);
  assert(myNormLoad <= 1.0);

  //
  // save delay
  //

  const auto myDelay = theClock - theServers[aServerIndex].theLastArrival;
  VLOG(2) << "server#" << aServerIndex << " delay " << myDelay << " load "
          << myNormLoad << " stat " << (theStatEnabled ? "" : "not ")
          << "enabled";

  if (theStatEnabled) {
    // summary stats
    theDelayStat(myDelay);

    // distributions per load
    const auto myDelayIndex =
        std::min(theDelays.size() - 1,
                 static_cast<size_t>(myNormLoad * theDelays.size()));
    theDelays[myDelayIndex].push_back(myDelay);
  }

  //
  // if there is at least one waiting customer serve it here (in FIFO order)
  //

  if (not theQueue.empty()) {
    theServers[aServerIndex].theLastArrival = theQueue.front();
    theQueue.pop_front();

    // add a new event for when this customer will leave from server
    const auto myDepartureTime =
        theClock + theExpos[aServerIndex](theGenerators[aServerIndex]);
    theEvents.emplace(Event(myDepartureTime, aServerIndex));

  } else {
    theServers[aServerIndex].theBusy = false;
    theBusyServers--;
    theLoads.push_back({theClock, theBusyServers});
  }
}

void MMC::sortDelays() {
  for (auto& myDeque : theDelays) {
    std::sort(myDeque.begin(), myDeque.end());
  }
}

} // namespace markov
} // namespace uiiit
