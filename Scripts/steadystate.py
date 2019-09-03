"""Compute the steady-state delays of a serverless edge computing"""

import numpy as np
from itertools import product
import scipy.sparse as sp
from scipy.linalg import norm

class DegenerateException(Exception):
    """Raised when the transition matrix is degenerate"""
    pass

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

        # lazy initialization variables
        self.delta    = None
        self.deltabar = None
        self.Q        = None
        self.pi       = None
        self.delays   = None

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

    def clear(self):
        "Remove all derived data structures"

        self.delta    = None
        self.deltabar = None
        self.Q        = None
        self.pi       = None
        self.delays   = None

    @staticmethod
    def printMat(name, mat):
        "Print a matrix per row, prepending the data structure name in a separate line"

        print "{}: ".format(name)
        if isinstance(mat, sp.dok_matrix):
            for row in mat.toarray():
                print row
        else:
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
            self.printMat("Average delays per state (serving)", self.__delta())
            self.printMat("Average delays per state(probing)", self.__deltabar())
            try:
                self.printMat("Steady state state transition matrix", self.transition())
                self.printMat("Steady state state probabilities", self.probabilities())
                self.printMat("Steady state average delays", self.steady_state_delays())
            except DegenerateException:
                self.clear()

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

    def __delta(self):
        "Compute the average delays when being server"

        if self.delta is not None:
            return self.delta

        self.delta = np.zeros([self.nclients, self.nstates])

        for i in range(self.nclients):
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

                self.delta[i, k] = self.tau[i, server] + numerator / denominator if denominator > 0 else -1

        return self.delta

    def __deltabar(self):
        "Compute the average delays when probing"

        if self.deltabar is not None:
            return self.deltabar

        self.deltabar = np.zeros([self.nclients, self.nstates])

        for i in range(self.nclients):
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

                self.deltabar[i, k] = self.tau[i, server] + numerator / denominator if denominator > 0 else -1

        return self.deltabar

    def transition(self):
        "Compute the transition matrix"

        if self.Q is not None:
            return self.Q

        # make sure the self.delta and self.deltabar variables are initialized
        self.__delta()
        self.__deltabar()

        # create an empty sparse matrix
        self.Q = sp.dok_matrix((self.nstates,self.nstates))

        # fill the sparse matrix Q with the state transition probabilities
        for k in range(self.nstates):
            # for every client, the possible servers to which it may go
            possible_destinations = []
            for i in range(self.nclients):
                if self.__remain(i, k):
                    possible_destinations.append([self.state[i, k]])
                else:
                    possible_destinations.append([self.state[i, k], self.statebar[i, k]])

            # identify the list of possible destination states, not including itself
            dest_states = []
            for h in range(self.nstates):
                to_be_added = True
                if h == k:
                    to_be_added = False
                else:
                    for i in range(self.nclients):
                        if self.state[i, h] not in possible_destinations[i] or \
                           self.delta[i, h] < 0:
                            to_be_added = False
                            break
                if to_be_added:
                    dest_states.append(h)
            
            # if there are no possible destinations, then k is an absorbing state,
            # which should not be the case
            if len(dest_states) == 0:
                raise DegenerateException("Cannot compute transition matrix with absorbing states")

            # we assume any state has the same probability to be reached from this
            state_prob = 1.0 / len(dest_states)
            for h in dest_states:
                self.Q[k, h] = state_prob

            # the transition matrix has zero-sum per row
            self.Q[k, k] = -1.0

        return self.Q


    #
    # copied from the SciPy cookbook
    # file: tandemqueue.py
    # the method is originally called computePiMethod1()
    #
    # https://scipy-cookbook.readthedocs.io/items/Solving_Large_Markov_Chains.html
    #
    def probabilities(self):
        "Compute the steady state probabilities"

        if self.pi is not None:
            return self.pi

        # make sure the self.Q variable is initialized
        self.transition()

        size = self.nstates
        l = min(self.Q.values())*1.001  # avoid periodicity, see trivedi's book
        P = sp.eye(size, size) - self.Q/l
        # compute Pi
        P =  P.tocsr()
        self.pi = np.zeros(size)
        pi1 = np.zeros(size)
        self.pi[0] = 1;
        n = norm(self.pi - pi1, 1)
        iterations = 0
        while n > 1e-3 and iterations < 1e5:
            pi1 = self.pi*P
            self.pi = pi1*P   # avoid copying pi1 to pi
            n = norm(self.pi - pi1, 1)
            iterations += 1

        return self.pi

    def __remain(self, i, k):
        "Return True if the client i prefers to remain when in state k"

        assert self.delta is not None
        assert self.deltabar is not None

        if self.deltabar[i, k] < 0 and self.delta[i, k] > 0:
            return True
        if self.delta[i, k] < 0 or self.deltabar[i, k] <= self.delta[i, k]:
            return False
        return True

    def absorbing(self):
        "Return the list of absorbing states (may be empty)"

        if self.delta is None:
            self.__delta()
        if self.deltabar is None:
            self.__deltabar();
        ret = []
        for k in range(self.nstates):
            serving_faster = True
            for i in range(self.nclients):
                serving_faster &= self.__remain(i, k)
            if serving_faster:
                ret.append(k)

        return ret

    def steady_state_delays(self):
        "Return the average delay per client"

        if self.delays is not None:
            return self.delays

        # make sure the delta and state probabilities are initialized
        self.__delta()
        self.probabilities()

        self.delays = np.matmul(self.delta, self.pi)

        return self.delays
