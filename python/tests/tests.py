import sys
sys.path.append('..')

from huffman import TreeEncoding, HuffmanEncoding
from huffman_ga import GrowingTreeAdaptiveHuffmanEncoding
from huffman_fta import FullTreeAdaptiveHuffmanEncoding
from huffman_util import build_tree, build_codebook, encode_data, decode_data
from tree import Node, Leaf, Branch
from util import group, pad, from_bits, bits, bits_needed
from freqtable import FrequencyTable
from move_to_front import MoveToFrontEncoding
from prediction import PredictionEncoding
from burrows_wheeler import BurrowsWheeler
from unpack import UnpackEncoding
from eof_encoding import EofEncoding
from bit_grouper import BitGrouperEncoding
from oracles import MemoryOracle, MarkovOracle, ConstantOracle, RepeatOracle


def test_build_tree():
    def check(expected, d):
        frequencies : FrequencyTable[str] = FrequencyTable[str]()
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
    def check(bytes):
        encoding = UnpackEncoding()
        packed = encoding.encode(bytes)
        unpacked = encoding.decode(packed)
        assert bytes == unpacked, repr(unpacked)
    yield check, b""
    yield check, b"\0"
    yield check, b"\0\1"
    yield check, b"\0\1\255"
    yield check, b"abcde456789"


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
        data = [ *data.encode('ascii') ]
        encoded = encoding.encode(data)
        decoded = list(encoding.decode(encoded))
        assert data == decoded, f'data={data}, decoded={decoded}'
    yield check, 'abc'
    yield check, 'aabbbc'
    yield check, 'aababcabcdabcde'
    yield check, 'abbccccdddddddd'
    yield check, 'acndjlkajcipocidjfdjslkfjsfjoijciojdiocjaiojcoisdjiojaiocjiojoijcio' * 1000


def test_predictions():
    def check(data, oracle_factory):
        encoding = PredictionEncoding(oracle_factory, 256)
        encoded = encoding.encode(data)
        restored = list(encoding.decode(encoded))
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
        yield check, [0], oracle
        yield check, [0, 1], oracle
        yield check, [0, 1, 2, 3, 4], oracle
        yield check, [5, 1, 2, 9, 4], oracle
        yield check, [255, 128, 254, 0], oracle
        yield check, [1, 1, 1, 1, 1, 1, 1, 1], oracle


def test_burrows_wheeler():
    def check(data, nvalues):
        encoding = BurrowsWheeler(nvalues)
        transformed = encoding.encode(data)
        untransformed = list(encoding.decode(transformed))
        assert data == untransformed
    yield check, [ 0 ], 2
    yield check, [ 1 ], 2
    yield check, [ 0, 1 ], 2
    yield check, [ 1, 0 ], 2
    yield check, [ 1, 0, 5, 100, 8, 52 ], 256
    yield check, [ 0, 255, 4, 3, 1, 9 ], 256


def test_move_to_front():
    encoding = MoveToFrontEncoding(256)
    def check(data):
        encoded = encoding.encode(data)
        decoded = list(encoding.decode(encoded))
        assert data == decoded, f'{data} != {decoded}'
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
        MoveToFrontEncoding(256),
        MoveToFrontEncoding(256) | MoveToFrontEncoding(256),
        MoveToFrontEncoding(256) | BurrowsWheeler(256),
        BurrowsWheeler(256) | MoveToFrontEncoding(257),
        HuffmanEncoding(256),
        EofEncoding(256) | HuffmanEncoding(257),
        EofEncoding(256) | HuffmanEncoding(257) | BitGrouperEncoding(8)
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
    def check(data, nvalues):
        encoding = EofEncoding(nvalues)
        encoded = encoding.encode(data)
        decoded = list(encoding.decode(encoded))
        assert data == decoded, f'data={data}, decoded={decoded}'

    yield check, [], 4
    yield check, [0], 4
    yield check, [1], 4
    yield check, [1, 1], 4
    yield check, [1, 2, 3, 4, 3, 2, 1], 16
    yield check, list(range(100)), 100


def test_adaptive_huffman_encoding_with_growing_tree():
    def check(data):
        encoding = GrowingTreeAdaptiveHuffmanEncoding(256)
        data = [ *data.encode('ascii') ]
        encoded = encoding.encode(data)
        decoded = list(encoding.decode(encoded))
        assert data == decoded, f'data={data}, decoded={decoded}'
    yield check, 'abc'
    yield check, 'aabbbc'
    yield check, 'aababcabcdabcde'
    yield check, 'abbccccdddddddd'
    yield check, 'acndjlkajcipocidjfdjslkfjsfjoijciojdiocjaiojcoisdjiojaiocjiojoijcio' * 10


def test_adaptive_huffman_encoding_with_full_tree():
    def check(data):
        encoding = FullTreeAdaptiveHuffmanEncoding(256)
        data = [ *data.encode('ascii') ]
        encoded = encoding.encode(data)
        decoded = list(encoding.decode(encoded))
        assert data == decoded, f'data={data}, decoded={decoded}'
    yield check, 'abc'
    yield check, 'aabbbc'
    yield check, 'aababcabcdabcde'
    yield check, 'abbccccdddddddd'
    yield check, 'acndjlkajcipocidjfdjslkfjsfjoijciojdiocjaiojcoisdjiojaiocjiojoijcio'


def test_encoding_inverter():
    def check(encoding, data):
        e = encoding | ~encoding
        encoded = e.encode(data)
        decoded = list(e.decode(encoded))
        assert data == decoded

    yield check, PredictionEncoding(lambda: MemoryOracle(2, 0), 10), []
    yield check, PredictionEncoding(lambda: MemoryOracle(2, 0), 10), [1, 1, 1, 1, 1]
    yield check, PredictionEncoding(lambda: MemoryOracle(2, 0), 10), [1, 2, 1, 3, 1, 4, 1, 5]
    yield check, MoveToFrontEncoding(10), []
    yield check, MoveToFrontEncoding(10), [0]
    yield check, MoveToFrontEncoding(10), [0, 0, 0, 0, 0]
    yield check, MoveToFrontEncoding(10), [1, 1, 1, 1, 1]
