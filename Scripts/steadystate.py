"""Compute the steady-state delays of a serverless edge computing"""

import numpy as np
from itertools import product

class SteadyState(object):
    """Steady-state delays of a serverless edge computing with two options"""

    def __init__(self,
                 chi,
                 tau,
                 x,
                 load,
                 mu,
                 association,
                 verbose = False):

        assert 0 < chi < 1
        assert len(tau.shape) == 2
        assert len(x.shape) == 1
        assert len(load.shape) == 1
        assert len(mu.shape) == 1
        assert len(association.shape) == 2

        # configuration
        self.verbose     = verbose

        # input
        self.chi         = chi
        self.tau         = tau
        self.x           = x
        self.load        = load
        self.mu          = mu
        self.association = association

        #
        # derived variables
        #

        # scalars
        self.nclients = tau.shape[0]
        self.nservers = tau.shape[1]
        self.nstates  = pow(2, self.nclients)

        # possible servers for each client
        possible_servers = []
        for i in range(self.nclients):
            server_list = []
            for (ndx,s) in zip(range(self.nservers),self.association[i]):
                if s == 1:
                    server_list.append(ndx)
            assert len(server_list) == 2
            possible_servers.append(server_list)
        self.possible_servers = np.array(possible_servers)

        # server assigned to every client per state
        self.state = np.zeros([self.nclients, self.nstates], dtype=int)
        self.statebar = np.zeros([self.nclients, self.nstates], dtype=int)
        for ndx, prod in zip(range(self.nstates), product([0, 1], repeat=self.nclients)):
            for i in range(self.nclients):
                self.state[i, ndx] = self.possible_servers[i][prod[i]]
                self.statebar[i, ndx] = self.possible_servers[i][1-prod[i]]

        # further size checks
        assert self.x.shape[0] == self.nclients
        assert self.load.shape[0] == self.nclients
        assert self.mu.shape[0] == self.nservers
        assert self.association.shape[0] == self.nclients
        assert self.association.shape[1] == self.nservers

    @staticmethod
    def printMat(name, mat):
        "Print a matrix per row, prepending the data structure name in a separate line"

        print "{}: ".format(name)
        for row in mat:
            print row

    def debugPrint(self, printDelay = False):
        "Print the internal data structures"

        self.printMat("Network delays",   self.tau)
        self.printMat("Requests",         self.x)
        self.printMat("Request rates",    self.load)
        self.printMat("Server rates",     self.mu)
        self.printMat("Associations",     self.association)
        self.printMat("Primary state",    self.state)
        self.printMat("Probe state",      self.statebar)
        self.printMat("Possible servers", self.possible_servers)

        if printDelay:
            self.printMat("Steady state average delays (serving)", self.delays())
            self.printMat("Steady state average delays (probing)", self.delaysbar())

    def I(self, client, server, state):
        "Return 1 if the client is served by server in a given state"

        if self.state[client, state] == server:
            return 1.0
        return 0.0

    def Ibar(self, client, server, state):
        "Return 1 if the client is probing server in a given state"

        if self.statebar[client, state] == server:
            return 1.0
        return 0.0

    def delays(self):
        "Compute the average delays when being server"

        delta = np.zeros([self.nclients, self.nstates])

        for i in range(self.nclients):
            #if self.verbose:
                #print "delays {}/{}".format(i, self.nclients)
            for k in range(self.nstates):
                server = self.state[i, k]
                assert 0 <= server < self.nservers

                numerator = self.x[i] * self.mu[server]
                denominator = self.mu[server]
                denominator -= self.load[i]
                for h in range(self.nclients):
                    if h == i:
                        continue
                    denominator -= \
                        self.load[h] * ( self.I(h, server, k) + self.chi * self.Ibar(h, server, k) )

                delta[i, k] = self.tau[i, server] + numerator / denominator if denominator > 0 else -1

        return delta

    def delaysbar(self):
        "Compute the average delays when probing"

        delta = np.zeros([self.nclients, self.nstates])

        for i in range(self.nclients):
            #if self.verbose:
                #print "delaysbar {}/{}".format(i, self.nclients)
            for k in range(self.nstates):
                server = self.statebar[i, k]
                assert 0 <= server < self.nservers

                numerator = self.x[i] * self.mu[server]
                denominator = self.mu[server]
                denominator -= self.chi * self.load[i]
                for h in range(self.nclients):
                    if h == i:
                        continue
                    denominator -= \
                        self.load[h] * ( self.I(h, server, k) + self.chi * self.Ibar(h, server, k) )

                delta[i, k] = self.tau[i, server] + numerator / denominator if denominator > 0 else -1

        return delta

    def absorbing(self):
        "Return the list of absorbing states (may be empty)"

        delta = self.delays()
        deltabar = self.delaysbar()
        ret = []
        for k in range(self.nstates):
            serving_faster = True
            for i in range(self.nclients):
                if deltabar[i, k] < 0 and delta[i, k] > 0:
                    continue
                if delta[i, k] < 0 or deltabar[i, k] <= delta[i, k]:
                    serving_faster = False
            if serving_faster:
                ret.append(k)

        return ret

