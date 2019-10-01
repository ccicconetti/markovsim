__author__  = "Claudio Cicconetti"
__version__ = "0.1.0"
__license__ = "MIT"

import numpy as np
import steadystate
import configuration

chi = 0.1
tau = np.array([[1, 1, 2], [1, 1, 2]])
x = np.array([1, 1])
load = np.array([3, 4.5])
mu = np.array([5, 10, 15])
association = np.array([[1, 1, 0], [0, 1, 1]])

ss = steadystate.SteadyState(
    configuration.Configuration(chi, tau, x, load, mu, association))

ss.debugPrint(True)
