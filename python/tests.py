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


def test_build_codebook():
    def check(expected, tree):
        actual = build_codebook(tree)
        assert expected == actual, repr(actual)
    yield check, { 'a': [0], 'b': [1] }, Branch( Leaf('a'), Leaf('b') )
    yield check, { 'a': [0, 0], 'b': [0, 1], 'c': [1, 0], 'd': [1, 1] }, Branch( Branch( Leaf('a'), Leaf('b') ), Branch( Leaf('c'), Leaf('d') ) )


def test_tree_encoding():
    def check(tree):
        encoded = encode_tree(tree)
        decoded = decode_tree(encoded)
        assert tree == decoded
    def l(*bits):
        return Leaf(list(bits))
    def b(left, right):
        return Branch(left, right)
    yield check, l(0)
    yield check, l(1)
    yield check, l(0,0)
    yield check, l(0,1)
    yield check, l(1,0,0,1,0,1)
    yield check, b(l(0), l(1))
    yield check, b(l(0), l(1,0))
    yield check, b(b(l(0), l(1)), l(1,0,1))


def test_packing():
    def check(bs):
        packed = pack(bs)
        unpacked = unpack(packed)
        assert bs == unpacked, repr(unpacked)
    yield check, [0]
    yield check, [1]
    yield check, [255]
    yield check, list(range(100))



def test_encode_data():
    def check(expected, data, book):
        assert expected == encode_data(data, book)
    yield check, [0], 'a', { 'a': [0] }
    yield check, [0, 0], 'aa', { 'a': [0] }
    yield check, [0, 1], 'ab', { 'a': [0], 'b': [1] }
    yield check, [1, 0, 1], 'bab', { 'a': [0], 'b': [1] }
    yield check, [1, 0, 0, 1, 0], 'bab', { 'a': [0], 'b': [1, 0] }
