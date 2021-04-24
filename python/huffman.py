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


T = TypeVar('T')
U = TypeVar('U')


class TreeEncoding:
    BRANCH_BIT : Bit = 0
    LEAF_BIT : Bit = 1

    __bits_per_datum : int

    def __init__(self, bits_per_datum : int):
        self.__bits_per_datum = bits_per_datum

    def encode(self, tree : Node[Datum]) -> Iterable[Bit]:
        if isinstance(tree, Leaf):
            yield TreeEncoding.LEAF_BIT
            yield from bits(tree.datum, self.__bits_per_datum)
        else:
            assert isinstance(tree, Branch)
            yield TreeEncoding.BRANCH_BIT
            yield from self.encode(tree.left)
            yield from self.encode(tree.right)

    def decode(self, bits : Iterator[Bit]) -> Node[Datum]:
        if next(bits) == TreeEncoding.BRANCH_BIT:
            left = self.decode(bits)
            right = self.decode(bits)
            return Branch(left, right)
        else:
            datum = from_bits(take(bits, self.__bits_per_datum))
            return Leaf(datum)


class HuffmanEncoding(Encoding[Data, Iterable[Bit]]):
    __tree_encoding : TreeEncoding
    __nvalues : int

    def __init__(self, nvalues : int):
        assert 0 < nvalues
        self.__nvalues = nvalues
        self.__tree_encoding = TreeEncoding(bits_needed(nvalues))

    def encode(self, data : Data) -> Iterable[Bit]:
        values = list(data)
        frequencies : FrequencyTable[Datum] = FrequencyTable.count_from_iterable(values)
        tree : Node[Datum] = build_tree(frequencies)
        codes : dict[Datum, list[Bit]] = build_codebook(tree)
        yield from self.__tree_encoding.encode(tree)
        yield from encode_data(values, codes)

    def decode(self, bits : Iterable[Bit]) -> Data:
        iterator = iter(bits)
        tree : Node[Datum] = self.__tree_encoding.decode(iterator)
        decoded = decode_data(iterator, tree)
        return decoded
