import unittest
import numpy as np
import steadystate

class TestSteadyState(unittest.TestCase):

    def test_simple_topology(self):
        chi = 0.1
        tau = np.array([[0, 0], [0, 0]])
        x = np.array([1, 1])
        load = np.array([0.1, 0.1])
        mu = np.array([1, 1])
        association = np.array([[1, 1], [1, 1]])

        ss = steadystate.SteadyState(chi, tau, x, load, mu, association, False)

        self.assertEqual([], ss.absorbing())

        ss.mu = np.array([1, 12])
        self.assertEqual([], ss.absorbing())
        ss.clear()
        self.assertEqual([3], ss.absorbing())

        ss.mu = np.array([12, 1])
        self.assertEqual([3], ss.absorbing())
        ss.clear()
        self.assertEqual([0], ss.absorbing())

if __name__ == '__main__':
    unittest.main()
