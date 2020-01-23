#!/usr/bin/python
"""Print the steady-state delays of a serverless edge computing"""

__author__  = "Claudio Cicconetti"
__version__ = "0.1.0"
__license__ = "MIT"

import argparse
import steadystate
import configuration
import numpy as np
import random 

parser = argparse.ArgumentParser(
    description=__doc__,
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument(
    "--verbose", action="store_true", default=False,
    help="Be verbose")
parser.add_argument(
    "--single", action="store_true", default=False,
    help="Single client-server association (cannot be used with --absorbing)")
parser.add_argument(
    "--absorbing", action="store_true", default=False,
    help="Print the number of absorbing state (cannot be used with --single)")
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
parser.add_argument(
    "--threads", type=int, default=1,
    help="Number of threads to use")
args = parser.parse_args()

# consistency checks
assert args.clients >= 1
assert args.servers >= 1
assert args.load_max >= args.load_min
assert args.mu_max >= args.mu_min
assert not (args.single and args.absorbing)

# initialize RNG
random.seed(args.seed)

# no network delay
tau = np.zeros([args.clients, args.servers])

# same task on all clients
x = np.ones([args.clients])

# create the configurations for all the runs
num_servers_per_client = 1 if args.single else 2
configurations = []
skipped = 0
for n in range(args.runs):
    # random serving rate
    mu = np.array([random.uniform(args.mu_min, args.mu_max) for i in range(args.servers)])

    # random load for clients
    load = np.array([random.uniform(args.load_min, args.load_max) for i in range(args.clients)])

    # random association
    association = np.zeros([args.clients, args.servers], dtype = int)
    for i in range(args.clients):
        for j in random.sample(range(args.servers), num_servers_per_client):
            association[i, j] = 1

    # skip runs, if requested by the user
    if args.skip_runs > skipped:
        print "skipped run#{}".format(n)
        skipped += 1
        continue

    configurations.append(configuration.Configuration(
        chi = args.chi,
        tau = tau,
        x = x,
        load = load,
        mu = mu,
        association = association))

if args.absorbing:
    num_absorbing = 0
    for conf in configurations:
        ss = steadystate.SteadyState(conf, args.verbose)
        absorbing_states = ss.absorbing()
        if len(absorbing_states) > 0:
            if len(absorbing_states) > 1:
                print "> 1 absorbing states: {}".format(absorbing_states)
            num_absorbing += 1
    with open(args.output, 'w') as outfile:
        outfile.write('{}\n'.format(num_absorbing))

else:
    sim = steadystate.Simulator(
        single = args.single,
        nthreads = args.threads,
        verbose = args.verbose,
        progress = args.progress)

    sim.run(configurations)

    with open(args.output, 'w') as outfile:
        for array in sim.average_delays:
            if array is None:
                # skip invalid measurements
                continue
            for value in array:
                outfile.write('{} '.format(value))
            outfile.write('\n')

