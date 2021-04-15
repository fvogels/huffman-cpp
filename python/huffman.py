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
import cProfile


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


def build_tree(frequencies : FrequencyTable[T]) -> Node[T]:
    def weight(node : Node[tuple[T, int]]) -> int:
        if isinstance(node, Leaf):
            return node.datum[1]
        else:
            assert isinstance(node, Branch)
            return weight(node.left) + weight(node.right)

    assert len(frequencies) > 0
    queue : list[Node[tuple[T, int]]] = [ Leaf((datum, weight)) for datum, weight in frequencies.items ]
    while len(queue) > 1:
        queue.sort(key=weight)
        branch = Branch(queue.pop(0), queue.pop(0))
        queue.append(branch)
    root = queue[0]
    return root.map(lambda p: p[0])


def build_codebook(tree : Node[T]) -> dict[T, list[Bit]]:
    def build(node : Node[T], prefix : list[Bit], book : dict[T, list[Bit]]) -> None:
        if isinstance(node, Leaf):
            book[node.datum] = prefix
        else:
            assert isinstance(node, Branch)
            build(node.left, [*prefix, 0], book)
            build(node.right, [*prefix, 1], book)
    result : dict[T, list[Bit]] = {}
    build(tree, [], result)
    return result


def encode_data(xs : Iterable[T], book : dict[T, list[Bit]]) -> Iterable[Bit]:
    return (bit for x in xs for bit in book[x])


def decode_data(bits : Iterator[Bit], root : Node[Datum]) -> Iterable[Datum]:
    assert isinstance(root, Branch)
    current_node : Node[Datum] = root
    while (bit := next(bits, None)) != None:
        assert isinstance(current_node, Branch)
        current_node = current_node.left if bit == 0 else current_node.right
        if isinstance(current_node, Leaf):
            datum = current_node.datum
            current_node = root
            yield datum






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





class BitGrouperEncoding(Encoding[Iterable[Bit], Data]):
    __nbits : int

    def __init__(self, nbits : int):
        assert nbits > 0
        self.__nbits = nbits

    def encode(self, bits : Iterable[Bit]) -> Data:
        bitcount = 0
        acc = 0
        for bit in bits:
            assert bit == 0 or bit == 1
            acc = acc * 2 + bit
            bitcount += 1
            if bitcount == self.__nbits:
                yield acc
                acc = 0
                bitcount = 0
        while bitcount < self.__nbits:
            acc *= 2
            bitcount += 1
        yield acc

    def decode(self, data : Data) -> Iterable[Bit]:
        for byte in data:
            for bit in bits(byte, self.__nbits):
                yield bit





class UnpackEncoding(Encoding[bytes, Data]):
    def encode(self, bs : bytes) -> Data:
        return [ t[0] for t in struct.iter_unpack('B', bs) ]

    def decode(self, ns : Data) -> bytes:
        data = list(ns)
        return struct.pack('B' * len(data), *data)


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
