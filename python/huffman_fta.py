from __future__ import annotations
import typing
from typing import Any, TypeVar, Union, Literal, Generic
from math import ceil, log2
from collections.abc import Iterable, Iterator, Callable
from abc import *
from freqtable import FrequencyTable
import struct
from util import Bit, bits, from_bits, bits_needed, take, append
from encoding import Encoding
from tree import Node, Leaf, Branch
from defs import Datum, Data
from huffman_util import build_tree, build_codebook, encode_data, decode_data


class FullTreeAdaptiveHuffmanEncoding(Encoding[Data, Iterable[Bit]]):
    __nvalues : int

    def __init__(self, nvalues : int):
        assert nvalues > 1
        self.__nvalues = nvalues

    def encode(self, data : Data) -> Iterable[Bit]:
        frequencies : FrequencyTable[Datum] = FrequencyTable.with_domain(range(self.__nvalues))

        for datum in data:
            assert 0 <= datum < self.__nvalues
            tree : Node[Datum] = build_tree(frequencies)
            codes : dict[Datum, list[Bit]] = build_codebook(tree)
            yield from codes[datum]
            frequencies.increment(datum)

    def decode(self, bits : Iterable[Bit]) -> Data:
        frequencies : FrequencyTable[Datum] = FrequencyTable.with_domain(range(self.__nvalues))
        current_node = build_tree(frequencies)
        end_reached = False

        for bit in bits:
            assert isinstance(current_node, Branch)
            current_node = current_node.left if bit == 0 else current_node.right
            if isinstance(current_node, Leaf):
                yield current_node.datum
                frequencies.increment(current_node.datum)
                current_node = build_tree(frequencies)
