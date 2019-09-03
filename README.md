This repository contains two independent projects, both related
to the numerical simulations of systems modeled as Markov
processes:

- serverless (in Python): simulator of a serverless environment
- markovsim (in C++): generic M/M/c simulator

# serverless

A numerical tool to compute the steady-state average delay of clients in
a serverless environment, where the servers behave as M/M/1 processes
with Processor Sharing policy.

The network latency between every client and server is constant and
can be set by the user.

The simulator assumes that every client is associated to exactly two
servers and it collects a long-term average of the service delay,
which is used to decide whether to switch to the secondary server
to which it is assigned. In order to compare the delay between primary
and secondary servers, it is also assumed that the client performs
a fraction of its requests towards the secondary server (the
fraction being equal for all clients and called Chi in code).

## Dependencies

Uses Python 2.7. Requires numpy and scipy.

## Unit tests

```
cd Serverless
python -m unittest test_steadystate.TestSteadyState
```

# markovsim

A simple M/M/c simulator.

## Dependencies

Requires C++-14 support.
Uses uiiitsupport https://github.com/ccicconetti/support/.
Tested on Mac OS X and Linux.

## Build instructions

First, make sure you have [CMake](https://cmake.org/) (version >= 3.2), a recent C++ compiler and all the dependencies installed:

- [glog](https://github.com/google/glog)
- [Boost](https://www.boost.org/)

Very likely you can find packaged versions in your system's package repository.

Note that [gmock](https://github.com/google/googlemock) is also needed to compile the unit tests but, if everything goes right, it will be downloaded automatically by CMake (needless to say: you do need a working Internet connection for this step).

```
git clone git@github.com:ccicconetti/markovsim.git
git submodule update --init --recursive
```

Once everything is ready (assuming `clang++` is your compiler):

```
cd build/debug
../build.sh clang++
make
```

If you want to compile with optimizations and without assertions, change `debug` into `release` above.
This will disable the compilation of the unit tests (see below).

## Execution

After compilation, this shows the command-line options:

```
cd build/debug/Executables
./markovsim -h
```

Output:

```
Allowed options:
  -h [ --help ]                  produce help message
  --seed arg (=1567521176755220) Seed generator. If not specified use the
                                 number of seconds since Jan 1, 1970.
  --lambda arg (=1)              Arrival rate.
  --mu arg (=1)                  Serving rate.
  --c arg (=2)                   Number of servers.
  --warmup arg (=30)             The warm-up duration, in s.
  --duration arg (=60)           Simulation duration step, in s.
  --threshold arg (=0.001)       Convergence threshold, in s.
  --distributions arg            Write the full distributions on file with the
                                 given base name.
```

## Unit tests

```
cd build/debug/Test/
./testmain
```

