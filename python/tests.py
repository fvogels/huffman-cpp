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


def test_group():
    def check(expected, xs, group_size):
        assert expected == group(xs, group_size)
    yield check, [[1], [2], [3]], [1, 2, 3], 1
    yield check, [[1, 2], [3]], [1, 2, 3], 2
    yield check, [[1, 2], [3, 4]], [1, 2, 3, 4], 2
    yield check, [[1, 2], [3, 4], [5, 6]], [1, 2, 3, 4, 5, 6], 2


def test_bits():
    def check(expected, n, size):
        assert expected == bits(n, size)
    yield check, [0, 0, 0, 0], 0, 4
    yield check, [0, 0, 0, 1], 1, 4
    yield check, [0, 0, 1, 0], 2, 4
    yield check, [0, 1, 0], 2, 3
    yield check, [0, 0, 0, 1, 0], 2, 5
    yield check, [0, 0, 0, 1, 1], 3, 5


def test_from_bits():
    def check(expected, bits):
        assert expected == from_bits(bits)
    yield check, 0, [0]
    yield check, 1, [1]
    yield check, 1, [0, 1]
    yield check, 2, [0, 1, 0]
    yield check, 3, [0, 1, 1]
    yield check, 7, [1, 1, 1]


def test_shift():
    def check(expected_result, expected_list, xs, n):
        assert expected_result == shift(xs, n)
        assert expected_list == xs
    yield check, [], [1, 2, 3], [1, 2, 3], 0
    yield check, [1], [2, 3], [1, 2, 3], 1
    yield check, [1, 2], [3], [1, 2, 3], 2
    yield check, [1, 2, 3], [], [1, 2, 3], 3


def test_frequencies():
    def check(expected, xs):
        assert expected == frequencies(xs)
    yield check, {}, []
    yield check, {1: 1}, [1]
    yield check, {1: 2}, [1, 1]
    yield check, {2: 2}, [2, 2]
    yield check, {1: 1, 2: 1}, [1, 2]


def test_build_tree():
    def check(expected, freqs):
        assert expected == build_tree(freqs)
    yield check, Leaf(('a', 1)), { 'a': 1 }
    yield check, Leaf(('a', 2)), { 'a': 2 }
    yield check, Branch( Leaf(('a', 1)), Leaf(('b', 2)) ), { 'a': 1, 'b': 2 }
    yield check, Branch( Leaf(('b', 1)), Leaf(('a', 2)) ), { 'a': 2, 'b': 1 }


def test_drop_weights():
    def check(expected, tree):
        assert expected == drop_weights(tree)
    yield check, Leaf('a'), Leaf(('a', 1))
    yield check, Leaf('a'), Leaf(('a', 2))
    yield check, Branch( Leaf('a'), Leaf('b') ), Branch( Leaf(('a', 1)), Leaf(('b', 2)) )
    yield check, Branch( Leaf('b'), Leaf('a') ), Branch( Leaf(('b', 1)), Leaf(('a', 2)) )