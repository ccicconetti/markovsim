#!/usr/bin/python
"""Print the steady-state delays of a serverless edge computing"""

__author__  = "Claudio Cicconetti"
__version__ = "0.1.0"
__license__ = "MIT"

import argparse
import steadystate
import numpy as np
import random 
import time

parser = argparse.ArgumentParser(
    description=__doc__,
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument(
    "--verbose", action="store_true", default=False,
    help="Be verbose")
parser.add_argument(
    "--progress", action="store_true", default=False,
    help="Print progress")
parser.add_argument(
    "--seed", type=int, default=0,
    help="Random number generators' seed")
parser.add_argument(
    "--chi", type=float, default=0.1,
    help="Chi value, in (0,1)")
parser.add_argument(
    "--clients", type=int, default=2,
    help="Number of clients")
parser.add_argument(
    "--servers", type=int, default=2,
    help="Number of servers")
parser.add_argument(
    "--mu_min", type=float, default=1,
    help="Minimum serving rate, in 1/s")
parser.add_argument(
    "--mu_max", type=float, default=1,
    help="Maximum serving rate, in 1/s")
parser.add_argument(
    "--load_min", type=float, default=0.1,
    help="Minimum load, in 1/s")
parser.add_argument(
    "--load_max", type=float, default=0.3,
    help="Maximum load, in 1/s")
parser.add_argument(
    "--runs", type=int, default=1,
    help="Number of replications")
parser.add_argument(
    "--skip_runs", type=int, default=0,
    help="Number of replications to be skipped")
parser.add_argument(
    "--output", type=str, default='out',
    help="Output file")
args = parser.parse_args()

# consistency checks
assert args.clients >= 1
assert args.servers >= 1
assert args.load_max >= args.load_min

# initialize RNG
random.seed(args.seed)

# no network delay
tau = np.zeros([args.clients, args.servers])

# same task on all clients
x = np.ones([args.clients])

average_delays = []
skipped = 0
for n in range(args.runs):
    # random serving rate
    mu = np.array([random.uniform(args.mu_min, args.mu_max) for i in range(args.servers)])

    # random load for clients
    load = np.array([random.uniform(args.load_min, args.load_max) for i in range(args.clients)])

    # random association
    association = np.zeros([args.clients, args.servers], dtype = int)
    for i in range(args.clients):
        for j in random.sample(range(args.servers), 2):
            association[i, j] = 1

    # skip runs, if requested by the user
    if args.skip_runs > skipped:
        print "skipped run#{}".format(n)
        skipped += 1
        continue

    ss = steadystate.SteadyState(
        chi = args.chi,
        tau = tau,
        x = x,
        load = load,
        mu = mu,
        association = association,
        verbose = args.verbose)

    if args.verbose:
        ss.debugPrint(True)

    try:
        now = time.time()
        average_delays.append(ss.steady_state_delays())
        if args.progress:
            print "run#{}: {} s".format(n, time.time() - now)

    except steadystate.DegenerateException:
        print "skipped run#{}, absorbing states: {}".format(n, ', '.join([str(y) for y in ss.absorbing()]))

with open(args.output, 'w') as outfile:
    for array in average_delays:
        for value in array:
            outfile.write('{} '.format(value))
        outfile.write('\n')

