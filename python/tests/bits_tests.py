import sys
sys.path.append('..')

from util import bits


def test_bits():
    def check(expected, n, size):
        assert expected == bits(n, size)
    yield check, [0, 0, 0, 0], 0, 4
    yield check, [0, 0, 0, 1], 1, 4
    yield check, [0, 0, 1, 0], 2, 4
    yield check, [0, 1, 0], 2, 3
    yield check, [0, 0, 0, 1, 0], 2, 5
    yield check, [0, 0, 0, 1, 1], 3, 5
