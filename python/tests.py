from huffman import *


def test_padding():
    def check(expected, xs, length, padder):
        assert expected == pad(xs, length, padder)
    yield check, [], [], 0, 0
    yield check, [0], [], 1, 0
    yield check, [0, 0], [], 2, 0
    yield check, [1, 1], [], 2, 1
    yield check, [5, 1], [5], 2, 1
    yield check, [5, 5], [5, 5], 2, 1
    yield check, [5, 5, 3], [5, 5, 3], 2, 1
    yield check, [5, 5, 3, 0, 0], [5, 5, 3], 5, 0