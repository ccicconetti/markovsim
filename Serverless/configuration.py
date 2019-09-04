"""Compute the steady-state delays of a serverless edge computing"""

__author__  = "Claudio Cicconetti"
__version__ = "0.1.0"
__license__ = "MIT"

class DegenerateException(Exception):
    """Raised when the transition matrix is degenerate"""
    pass

class Configuration(object):
    """Configuration of a steady-state simulation"""

    def __init__(self,
                 chi,
                 tau,
                 x,
                 load,
                 mu,
                 association):

        # consistency checks
        assert 0 < chi < 1
        assert len(tau.shape) == 2
        assert len(x.shape) == 1
        assert len(load.shape) == 1
        assert len(mu.shape) == 1
        assert len(association.shape) == 2

        # save input values
        self.chi         = chi
        self.tau         = tau
        self.x           = x
        self.load        = load
        self.mu          = mu
        self.association = association
