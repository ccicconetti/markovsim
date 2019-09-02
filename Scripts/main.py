#!/usr/bin/python
"""Print the steady-state delays of a serverless edge computing"""

import sys
import argparse
import steadystate
import numpy as np

parser = argparse.ArgumentParser(
    description=__doc__,
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument(
    "--verbose", action="store_true", default=False,
    help="Be verbose")
args = parser.parse_args()

chi = 0.1
tau = np.array([[1, 2, 3], [4, 5, 6]])
x = np.array([1, 2])
load = np.array([0.1, 0.2])
mu = np.array([1, 2, 3])
association = np.array([[1, 1, 0], [0, 1, 1]])

ss = steadystate.SteadyState(
    chi = chi,
    tau = tau,
    x = x,
    load = load,
    mu = mu,
    association = association,
    verbose = args.verbose)

ss.debugPrint()

delta = ss.delays()
print "Steady state average delays (serving)"
for (i,row) in zip(range(ss.nclients),delta):
    print "{}: {}".format(i, row)


deltabar = ss.delaysbar()
print "Steady state average delays (probing)"
for (i,row) in zip(range(ss.nclients),deltabar):
    print "{}: {}".format(i, row)

print "Absorbing states: {}".format(', '.join([str(x) for x in ss.absorbing()]))
