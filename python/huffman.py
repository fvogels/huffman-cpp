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
from huffman_util import build_tree, build_codebook, encode_data, decode_data


T = TypeVar('T')
U = TypeVar('U')


class TreeEncoding:
    __bits_per_datum : int

    def __init__(self, bits_per_datum : int):
        self.__bits_per_datum = bits_per_datum

    def encode(self, tree : Node[Datum]) -> Iterable[Bit]:
        if isinstance(tree, Leaf):
            yield 1
            yield from bits(tree.datum, self.__bits_per_datum)
        else:
            assert isinstance(tree, Branch)
            yield 0
            yield from self.encode(tree.left)
            yield from self.encode(tree.right)

    def decode(self, bits : Iterator[Bit]) -> Node[Datum]:
        if next(bits) == 0:
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
