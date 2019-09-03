# serverless

A numerical tool to compute the steady-state average delay of clients in
a serverless environment, where the servers behave as M/M/1 processes
with Processor Sharing policy.

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

## Compilation

Assuming `clang++` is your compiler:

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

