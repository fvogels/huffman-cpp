import sys
sys.path.append('..')

from util import bits_needed


def test_bits_needed():
    def check(expected, n):
        actual = bits_needed(n)
        assert expected == actual
    yield check, 1, 2
    yield check, 2, 3
    yield check, 2, 4
    yield check, 3, 5
    yield check, 8, 2**8
