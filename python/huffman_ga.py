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


class GrowingTreeAdaptiveHuffmanEncoding(Encoding[Data, Iterable[Bit]]):
    __nvalues : int

    def __init__(self, nvalues : int):
        assert nvalues > 1
        self.__nvalues = nvalues

    def encode(self, data : Data) -> Iterable[Bit]:
        not_yet_transmitted = self.__nvalues
        frequencies : FrequencyTable[Datum] = FrequencyTable.with_domain([ not_yet_transmitted ])
        bn = bits_needed(self.__nvalues)

        for datum in data:
            tree : Node[Datum] = build_tree(frequencies)
            codes : dict[Datum, list[Bit]] = build_codebook(tree)
            if datum in codes:
                yield from codes[datum]
            else:
                yield from codes[not_yet_transmitted]
                yield from bits(datum, bn)
            frequencies.increment(datum)

    def decode(self, encoded : Iterable[Bit]) -> Data:
        bits = iter(encoded)
        not_yet_transmitted = self.__nvalues
        bn = bits_needed(self.__nvalues)
        frequencies : FrequencyTable[Datum] = FrequencyTable.with_domain([ not_yet_transmitted ])
        current_tree = build_tree(frequencies)
        end_reached = False

        while not end_reached:
            if isinstance(current_tree, Leaf):
                datum = current_tree.datum
                if datum == not_yet_transmitted:
                    datum = from_bits(take(bits, bn))
                yield datum
                frequencies.increment(datum)
                current_tree = build_tree(frequencies)
            else:
                assert isinstance(current_tree, Branch)
                if (bit := next(bits, None)) != None:
                    current_tree = current_tree.left if bit == 0 else current_tree.right
                else:
                    end_reached = True