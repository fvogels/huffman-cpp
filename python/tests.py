from huffman import *


def test_bits_needed():
    def check(expected, n):
        actual = bits_needed(n)
        assert expected == actual
    yield check, 1, 2
    yield check, 2, 3
    yield check, 2, 4
    yield check, 3, 5
    yield check, 8, 2**8


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
        encoding = TreeEncoding(4)
        encoded = encoding.encode(tree)
        decoded = encoding.decode(encoded)
        assert tree == decoded
    def l(value):
        return Leaf(value)
    def b(left, right):
        return Branch(left, right)
    yield check, l(0)
    yield check, l(1)
    yield check, l(5)
    yield check, l(15)
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
        assert expected == list(encode_data(data, book))
    yield check, [0], 'a', { 'a': [0] }
    yield check, [0, 0], 'aa', { 'a': [0] }
    yield check, [0, 1], 'ab', { 'a': [0], 'b': [1] }
    yield check, [1, 0, 1], 'bab', { 'a': [0], 'b': [1] }
    yield check, [1, 0, 0, 1, 0], 'bab', { 'a': [0], 'b': [1, 0] }


def test_decode_data():
    def check(expected, data, tree):
        expected = [*expected]
        actual = list(decode_data(iter(data), tree))
        assert expected == actual, f'actual={repr(actual)}, expected={expected}'
    def B(left, right):
        return Branch(left, right)

    a = Leaf('a')
    b = Leaf('b')
    c = Leaf('c')

    yield check, 'a', [0], B(a, b)
    yield check, 'aa', [0, 0], B(a, b)
    yield check, 'ab', [0, 1], B(a, b)
    yield check, 'a', [0, 0], B(B(a, b), c)
    yield check, 'b', [0, 1], B(B(a, b), c)
    yield check, 'c', [1], B(B(a, b), c)
    yield check, 'abc', [0,0,0,1,1], B(B(a, b), c)


def test_huffman_encoding():
    def check(data):
        encoding = HuffmanEncoding(256)
        data = Data(data.encode('ascii'), 256)
        encoded = encoding.encode(data)
        decoded = encoding.decode(encoded)
        assert data == decoded, f'data={data}, decoded={decoded}'
    yield check, 'abc'
    yield check, 'aabbbc'
    yield check, 'aababcabcdabcde'
    yield check, 'abbccccdddddddd'
    yield check, 'acndjlkajcipocidjfdjslkfjsfjoijciojdiocjaiojcoisdjiojaiocjiojoijcio' * 1000


def test_predictions():
    def check(ns, oracle_factory):
        data = Data(ns, max(ns) + 1)
        encoding = PredictionEncoding(oracle_factory)
        encoded = encoding.encode(data)
        restored = list(encoding.decode(encoded).values)
        assert restored == ns, f'{restored} != {ns}'

    oracles = [
        lambda: ConstantOracle(0),
        lambda: RepeatOracle(0),
        lambda: MarkovOracle(0),
        lambda: MemoryOracle(1, 0),
        lambda: MemoryOracle(2, 0),
        lambda: MemoryOracle(3, 0),
    ]

    for oracle in oracles:
        yield check, [0], oracle
        yield check, [0, 1], oracle
        yield check, [0, 1, 2, 3, 4], oracle
        yield check, [5, 1, 2, 9, 4], oracle
        yield check, [255, 128, 254, 0], oracle
        yield check, [1, 1, 1, 1, 1, 1, 1, 1], oracle


def test_burrows_wheeler():
    def check(ns):
        data = Data(ns, max(ns) + 1)
        encoding = BurrowsWheeler()
        transformed = encoding.encode(data)
        untransformed = list(encoding.decode(transformed).values)
        assert ns == untransformed
    yield check, [ 0 ]
    yield check, [ 1 ]
    yield check, [ 0, 1 ]
    yield check, [ 1, 0 ]
    yield check, [ 1, 0, 5, 100, 8, 52 ]


def test_data_encoding():
    def check(ns, nvalues):
        encoding = DataEncoding(nvalues)
        encoded = encoding.encode(ns)
        decoded = list(encoding.decode(encoded))
        assert ns == decoded, f'{data} != {decoded}'
    yield check, [], 2
    yield check, [0], 2
    yield check, [5], 10
    yield check, [2, 9], 10
    yield check, [5, 2, 9, 100], 256


def test_move_to_front():
    encoding = MoveToFrontEncoding()
    def check(ns):
        data = Data(ns, 256)
        encoded = encoding.encode(data)
        decoded = list(encoding.decode(encoded).values)
        assert ns == decoded, f'{ns} != {decoded}'
    yield check, []
    yield check, [0]
    yield check, [5]
    yield check, [2, 9]
    yield check, [5, 2, 9, 100]
    yield check, [8, 7, 5, 4, 3, 2, 1, 5, 7, 8, 9, 5, 6]


def test_encoder_combination():
    def check(encoding, data):
        encoded = encoding.encode(data)
        decoded = list(encoding.decode(encoded))
        assert data == decoded, f'data={data}, decoded={decoded}'

    encodings = [
        DataEncoding(256) | MoveToFrontEncoding(),
        DataEncoding(256) | MoveToFrontEncoding() | MoveToFrontEncoding(),
        DataEncoding(256) | MoveToFrontEncoding() | BurrowsWheeler(),
        DataEncoding(256) | BurrowsWheeler() | MoveToFrontEncoding(),
        DataEncoding(256) | HuffmanEncoding(256),
        DataEncoding(256) | EofEncoding(256) | HuffmanEncoding(257),
    ]
    for encoding in encodings:
        yield check, encoding, [1, 2, 3, 4]
        yield check, encoding, [5, 1, 3, 2, 6, 5, 8]
        yield check, encoding, [1, 2, 3] * 10


def test_bit_grouper():
    def check(bits, group_size):
        encoding = BitGrouperEncoding(group_size)
        encoded = encoding.encode(bits)
        decoded = list(encoding.decode(encoded))[0:len(bits)]
        assert bits == decoded, f'bits={bits}, decoded={decoded}'

    for group_size in [2, 4, 8, 12, 16]:
        yield check, [], group_size
        yield check, [0], group_size
        yield check, [1], group_size
        yield check, [1, 1], group_size
        yield check, [1, 0, 1, 0, 1, 1, 1, 0], group_size
        yield check, [0] * 8 * 10, group_size


def test_eof_encoding():
    def check(ns, nvalues):
        data = Data(ns, nvalues)
        encoding = EofEncoding(nvalues)
        encoded = encoding.encode(data)
        decoded = list(encoding.decode(encoded).values)
        assert ns == decoded, f'ns={ns}, decoded={decoded}'

    yield check, [], 4
    yield check, [0], 4
    yield check, [1], 4
    yield check, [1, 1], 4
    yield check, [1, 2, 3, 4, 3, 2, 1], 16
    yield check, list(range(100)), 100

# def test_encoding_inverter():
#     def check(encoding, data):
#         e = encoding | ~encoding
#         encoded = e.encode(data)
#         decoded = list(e.decode(encoded))
#         assert data == decoded

#     yield check, PredictionEncoding(lambda: MemoryOracle(2, 0)), []
#     yield check, PredictionEncoding(lambda: MemoryOracle(2, 0)), [1, 1, 1, 1, 1]
#     yield check, PredictionEncoding(lambda: MemoryOracle(2, 0)), [1, 2, 1, 3, 1, 4, 1, 5]
#     yield check, MoveToFrontEncoding(), []
#     yield check, MoveToFrontEncoding(), [0]
#     yield check, MoveToFrontEncoding(), [0, 0, 0, 0, 0]
#     yield check, MoveToFrontEncoding(), [1, 1, 1, 1, 1]
