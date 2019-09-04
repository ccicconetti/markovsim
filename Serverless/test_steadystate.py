__author__  = "Claudio Cicconetti"
__version__ = "0.1.0"
__license__ = "MIT"

import unittest
import numpy as np
import steadystate
import configuration

class TestSteadyState(unittest.TestCase):

    def test_absorbing(self):
        chi = 0.1
        tau = np.array([[0, 0], [0, 0]])
        x = np.array([1, 1])
        load = np.array([0.1, 0.1])
        mu = np.array([1, 1])
        association = np.array([[1, 1], [1, 1]])

        ss = steadystate.SteadyState(configuration.Configuration(chi, tau, x, load, mu, association), False)

        self.assertEqual([], ss.absorbing())

        ss.mu = np.array([1, 12])
        self.assertEqual([], ss.absorbing())
        ss.clear()
        self.assertEqual([3], ss.absorbing())

        ss.mu = np.array([12, 1])
        self.assertEqual([3], ss.absorbing())
        ss.clear()
        self.assertEqual([0], ss.absorbing())


    def test_steady_state_delays(self):
        chi = 0.5
        tau = np.array([[1, 1, 3], [2, 2, 1]])
        x = np.array([1, 1])
        load = np.array([0.2, 0.2])
        mu = np.array([1, 2, 1])
        association = np.array([[1, 1, 0], [0, 1, 1]])

        ss = steadystate.SteadyState(configuration.Configuration(chi, tau, x, load, mu, association), False)

        self.assertEqual([], ss.absorbing())

        expected = [2.213, 2.250]
        for e,a in zip(expected, ss.steady_state_delays()):
            self.assertAlmostEqual(e, a, 3)

if __name__ == '__main__':
    unittest.main()
