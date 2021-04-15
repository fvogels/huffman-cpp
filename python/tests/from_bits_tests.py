import sys
sys.path.append('..')

from util import from_bits


def test_from_bits():
    def check(expected, bits):
        assert expected == from_bits(bits)

    yield check, 0, [0]
    yield check, 1, [1]
    yield check, 1, [0, 1]
    yield check, 2, [0, 1, 0]
    yield check, 3, [0, 1, 1]
    yield check, 7, [1, 1, 1]
