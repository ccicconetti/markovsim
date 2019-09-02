"""Compute the steady-state delays of a serverless edge computing"""

import numpy as np
from itertools import product

class SteadyState(object):
    """Steady-state delays of a serverless edge computing with two options"""

    def __init__(self,
                 tau,
                 x,
                 load,
                 mu,
                 association,
                 verbose = False):

        assert len(tau.shape) == 2
        assert len(x.shape) == 1
        assert len(load.shape) == 1
        assert len(mu.shape) == 1
        assert len(association.shape) == 2

        # configuration
        self.verbose     = verbose

        # input
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
        print "{}: ".format(name)
        for row in mat:
            print row

    def debugPrint(self):
        self.printMat("Network delays",   self.tau)
        self.printMat("Requests",         self.x)
        self.printMat("Request rates",    self.load)
        self.printMat("Server rates",     self.mu)
        self.printMat("Associations",     self.association)
        self.printMat("Primary state",    self.state)
        self.printMat("Probe state",      self.statebar)
        self.printMat("Possible servers", self.possible_servers)
