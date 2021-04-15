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
