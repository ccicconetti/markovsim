This repo contains the source code and scripts used in the paper:

C. Puliafito, C. Cicconetti, M. Conti, E. Mingozzi and A. Passarella 

_Stateful Function as a Service at the Edge_," 

 in Computer, vol. 55, no. 9, pp. 54-64, Sept. 2022, doi: [10.1109/MC.2021.3138690](https://doi.org/10.1109/MC.2021.3138690).

# Building

## Prerequirements

- git
- recent cmake and C++ compiler

## Instructions

```
git clone https://github.com/ccicconetti/markovsim.git
cd markovsim/build/release
../build.sh g++
cd ../../simulations/000_StatefulFaaS
ln -s $OLDPWD/Executables/sfm* .
```

You should have now two symbolic links `sfm-provisioning` and `sfm-latency` in this directory.

# Run simulations

```
./run-latency.sh
./run-provisioning.sh
```

You should have now the results in `results/`.

# Visualization

Requires gnuplot:

```
cd graphs
for i in *.plt ; do gnuplot -persist $i ; done
```

