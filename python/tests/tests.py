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
