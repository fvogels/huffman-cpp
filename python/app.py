from __future__ import annotations
import typing
from typing import Any, TypeVar, Union, Literal, Generic
from math import ceil, log2
from collections.abc import Iterable, Iterator, Callable
from abc import *
from freqtable import FrequencyTable
import struct
from util import Bit, bits, from_bits, bits_needed, take, append
from oracles import Oracle, ConstantOracle, RepeatOracle, MarkovOracle, MemoryOracle
from encoding import Encoding
from tree import Node, Leaf, Branch
from defs import Datum, Data
from eof_encoding import EofEncoding
from bit_grouper import BitGrouperEncoding
from huffman import HuffmanEncoding
import cProfile



def main():
    encodings = [
        # (
        #     'Huffman',
        #     HuffmanEncoding(257)
        # ),
        # (
        #     'GT-Adaptive',
        #     GrowingTreeAdaptiveHuffmanEncoding(257)
        # ),
        # (
        #     'Repeat GT-Adaptive',
        #     PredictionEncoding(lambda: RepeatOracle(0), 257) | GrowingTreeAdaptiveHuffmanEncoding(257)
        # ),
        # (
        #     'Markov GT-Adaptive',
        #     PredictionEncoding(lambda: MarkovOracle(0), 257) | GrowingTreeAdaptiveHuffmanEncoding(257)
        # ),
        # (
        #     'M1 GT-Adaptive',
        #     PredictionEncoding(lambda: MemoryOracle(1, 0), 257) | GrowingTreeAdaptiveHuffmanEncoding(257)
        # ),
        # (
        #     'M2 GT-Adaptive',
        #     PredictionEncoding(lambda: MemoryOracle(2, 0), 257) | GrowingTreeAdaptiveHuffmanEncoding(257)
        # ),
        # (
        #     'M5 GT-Adaptive',
        #     PredictionEncoding(lambda: MemoryOracle(5, 0), 257) | GrowingTreeAdaptiveHuffmanEncoding(257)
        # ),
        (
            'FT-Adaptive',
            FullTreeAdaptiveHuffmanEncoding2(257)
        ),
    ]

    inputs = [
        b'\0' * 10**2,
        # b'\0\1' * 100,
        # b'abcdef' * 100,
        # b'ababbbcbcdbdebe' * 100,
        # b'Fruit flies like a banana' * 100
    ]

    for index, data in enumerate(inputs):
        original_size = len(data)
        print(f'Input #{index + 1}')
        print(f'Original size: {original_size}')
        for description, encoding in encodings:
            encoded = (UnpackEncoding() | EofEncoding(256) | encoding | BitGrouperEncoding(8) | ~UnpackEncoding()).encode(data)
            compressed_size = len(encoded)
            percentage = round((compressed_size / original_size - 1) * 100, 0)
            print(f'{description} -> {compressed_size} ({percentage}%)')
        print()


if __name__ == '__main__':
    cProfile.run('main()')
