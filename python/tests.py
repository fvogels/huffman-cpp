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


def test_datum_bit_conversion():
    def check(datum):
        bits = datum_to_bits(datum)
        decoded = bits_to_datum(bits)
        assert datum == decoded
    yield check, Eof()
    yield check, 0
    yield check, 1


def test_build_tree():
    def check(expected, d):
        frequencies = FrequencyTable()
        for value, n in d.items():
            for _ in range(n):
                frequencies.increment(value)

        assert expected == build_tree(frequencies)
    yield check, Leaf('a'), { 'a': 1 }
    yield check, Branch( Leaf('a'), Leaf('b') ), { 'a': 1, 'b': 2 }
    yield check, Branch( Leaf('b'), Leaf('a') ), { 'a': 2, 'b': 1 }


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
    def l(value):
        return Leaf(value)
    def b(left, right):
        return Branch(left, right)
    yield check, l(0)
    yield check, l(1)
    yield check, l(5)
    yield check, b(l(0), l(1))
    yield check, b(l(0), l(4))
    yield check, b(b(l(0), l(1)), l(9))


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


def test_decode_data():
    def check(expected, data, tree):
        data = iter([ *data, Eof() ])
        actual = list(decode_data(data, tree))
        assert [*expected, Eof()] == actual, repr(actual)
    def l(datum):
        return Leaf(datum)
    def b(left, right):
        return Branch(left, right)
    eof = Eof()
    yield check, '', [0], b(l(eof), l('b'))
    yield check, 'a', [0,1], b(l('a'), l(eof))
    yield check, 'aa', [0,0,1], b(l('a'), l(eof))
    yield check, 'ab', [0,0,0,1,1], b(b(l('a'), l('b')), l(eof))


def test_huffman_encoding():
    def check(data):
        data = [*data.encode('ascii'), Eof()]
        encoded = huffman_encode(data)
        decoded = list(huffman_decode(encoded))
        assert data == decoded, f'data={data}, decoded={decoded}'
    yield check, ''
    yield check, 'a'
    yield check, 'abc'
    yield check, 'aabbbc'
    yield check, 'aababcabcdabcde'
    yield check, 'abbccccdddddddd'


def test_predictions():
    def check(data, oracle_factory):
        deltas = predict(data, oracle_factory())
        restored = list(unpredict(deltas, oracle_factory()))
        assert restored == data, f'{restored} != {data}'

    oracles = [
        lambda: ConstantOracle(0),
        lambda: RepeatOracle(0),
        lambda: MarkovOracle(0),
        lambda: MemoryOracle(1, 0),
        lambda: MemoryOracle(2, 0),
        lambda: MemoryOracle(3, 0),
    ]

    for oracle in oracles:
        yield check, [], oracle
        yield check, [0], oracle
        yield check, [0, 1], oracle
        yield check, [0, 1, 2, 3, 4], oracle
        yield check, [5, 1, 2, 9, 4], oracle
        yield check, [255, 128, 254, 0], oracle
        yield check, [1, 1, 1, 1, 1, 1, 1, 1], oracle


def test_burrows_wheeler():
    def check(data):
        transformed = burrows_wheeler_transform(data)
        untransformed = burrows_wheeler_untransform(transformed)
        assert data == untransformed
    yield check, [ Eof() ]
    yield check, [ 0, Eof() ]
    yield check, [ 1, Eof() ]
    yield check, [ 0, 1, Eof() ]
    yield check, [ 1, 0, Eof() ]


def test_datum_encoding():
    encoding = DatumEncoding()
    def check(data):
        encoded = encoding.encode(data)
        decoded = list(encoding.decode(encoded))
        assert data == decoded, f'{data} != {decoded}'
    yield check, []
    yield check, [0]
    yield check, [5]
    yield check, [2, 9]
    yield check, [5, 2, 9, 100]
