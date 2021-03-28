from __future__ import annotations
from typing import Any, TypeVar, Union, Literal, Generic
from math import ceil, log2
from collections.abc import Iterable, Iterator, Callable
from abc import *
import struct
from freqtable import FrequencyTable
from util import Bit, bits, from_bits, bits_needed, take, rotate_left, append
from oracles import Oracle, ConstantOracle, RepeatOracle, MarkovOracle, MemoryOracle


T = TypeVar('T')
U = TypeVar('U')
V = TypeVar('V')


Datum = int
Data = Iterable[Datum]


class Node(Generic[T]):
    @abstractmethod
    def map(self, func : Callable[[T], U]) -> Node[U]:
        ...

    @abstractmethod
    def __eq__(self, other : Any) -> bool:
        ...

    @abstractmethod
    def __repr__(self) -> str:
        ...


class Branch(Node[T]):
    __left : Node[T]
    __right : Node[T]

    def __init__(self, left : Node[T], right : Node[T]):
        self.__left = left
        self.__right = right

    @property
    def left(self) -> Node[T]:
        return self.__left

    @property
    def right(self) -> Node[T]:
        return self.__right

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Branch) and self.left == other.left and self.right == other.right

    def map(self, func : Callable[[T], U]) -> Branch[U]:
        left : Node[U] = self.left.map(func)
        right : Node[U] = self.right.map(func)
        return Branch(left, right)

    def __repr__(self) -> str:
        return f"B[{self.left}|{self.right}]"


class Leaf(Node[T]):
    datum : T

    def __init__(self, datum : T):
        self.datum = datum

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Leaf) and self.datum == other.datum

    def map(self, func : Callable[[T], U]) -> Leaf[U]:
        return Leaf(func(self.datum))

    def __repr__(self) -> str:
        return f"L[{repr(self.datum)}]"


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




class Encoding(Generic[T,U]):
    def encode(self, x : T) -> U:
        raise NotImplementedError()

    def decode(self, x : U) -> T:
        raise NotImplementedError()

    def __or__(self, other : Encoding[U, V]) -> Encoding[T, V]:
        return EncodingCombinator(self, other)

    def __invert__(self) -> Encoding[U, T]:
        return EncodingInverter(self)



class EncodingCombinator(Generic[T, U, V], Encoding[T, V]):
    __left : Encoding[T, U]
    __right : Encoding[U, V]

    def __init__(self, left : Encoding[T, U], right : Encoding[U, V]):
        self.__left = left
        self.__right = right

    def encode(self, x : T) -> V:
        return self.__right.encode(self.__left.encode(x))

    def decode(self, x : V) -> T:
        return self.__left.decode(self.__right.decode(x))


class EncodingInverter(Encoding[U, T]):
    __encoding : Encoding[T, U]

    def __init__(self, encoding : Encoding[T, U]):
        self.__encoding = encoding

    def encode(self, x : U) -> T:
        return self.__encoding.decode(x)

    def decode(self, x : T) -> U:
        return self.__encoding.encode(x)


class PredictionEncoding(Encoding[Data, Data]):
    __nvalues : int

    def __init__(self, oracle_factory : Callable[[], Oracle], nvalues : int):
        assert oracle_factory is not None
        assert nvalues > 0
        self.__oracle_factory = oracle_factory
        self.__nvalues = nvalues

    def encode(self, data : Data) -> Data:
        assert data is not None
        oracle = self.__oracle_factory()
        for actual in data:
            assert 0 <= actual < self.__nvalues, f'actual={actual}, nvalues={self.__nvalues}'
            prediction = oracle.predict()
            oracle.tell(actual)
            correction = self.__compute_correction(prediction, actual)
            assert 0 <= correction < self.__nvalues
            yield correction

    def __compute_correction(self, prediction : Datum, actual : Datum) -> Datum:
        result = (actual - prediction) % self.__nvalues
        return result

    def decode(self, corrections : Data) -> Data:
        assert corrections is not None
        oracle = self.__oracle_factory()
        for correction in corrections:
            assert 0 <= correction < self.__nvalues
            prediction = oracle.predict()
            actual = self.__apply_correction(prediction, correction)
            oracle.tell(actual)
            assert 0 <= actual < self.__nvalues
            yield actual

    def __apply_correction(self, prediction : Datum, correction : Datum) -> Datum:
        result = (prediction + correction) % self.__nvalues
        return result


class BurrowsWheeler(Encoding[Data, Data]):
    __nvalues : int

    def __init__(self, nvalues : int):
        assert nvalues > 0
        self.__nvalues = nvalues

    def encode(self, data : Data) -> Data:
        xs = [ *data, self.__nvalues ]
        rotations = [ rotate_left(xs, i) for i in range(len(xs)) ]
        rotations.sort()
        return (rotation[-1] for rotation in rotations)

    def decode(self, data : Data) -> Data:
        xs = list(data)
        assert all(0 <= x < self.__nvalues+1 for x in xs)
        table : list[list[Datum]] = [ [] for _ in range(len(xs)) ]
        for _ in range(len(xs)):
            for row, datum in zip(table, xs):
                row.insert(0, datum)
            table.sort()
        result : Data = next(row for row in table if row[-1] == self.__nvalues)[:-1]
        assert all(0 <= x < self.__nvalues for x in result)
        return result


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


class MoveToFrontEncoding(Encoding[Data, Data]):
    __nvalues : int

    def __init__(self, nvalues : int):
        self.__nvalues = nvalues

    def encode(self, data : Data) -> Data:
        table = list(range(self.__nvalues))
        for datum in data:
            assert 0 <= datum < self.__nvalues, f'datum={datum}, nvalues={self.__nvalues}'
            index = table.index(datum)
            assert 0 <= index < self.__nvalues
            yield index
            del table[index]
            table.insert(0, datum)

    def decode(self, data : Data) -> Data:
        table = list(range(self.__nvalues))
        for x in data:
            assert 0 <= x < self.__nvalues
            value = table[x]
            assert 0 <= value <= self.__nvalues
            yield value
            del table[x]
            table.insert(0, value)


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


class EofEncoding(Encoding[Data, Data]):
    __nvalues : int

    def __init__(self, nvalues : int):
        self.__nvalues = nvalues

    def encode(self, data : Data) -> Data:
        eof = self.__nvalues
        return append(data, eof)

    def decode(self, data : Data) -> Data:
        eof = self.__nvalues
        values = iter(data)
        while (datum := next(values)) != eof:
            yield datum


class UnpackEncoding(Encoding[bytes, Data]):
    def encode(self, bs : bytes) -> Data:
        return [ t[0] for t in struct.iter_unpack('B', bs) ]

    def decode(self, ns : Data) -> bytes:
        data = list(ns)
        return struct.pack('B' * len(data), *data)


def main():
    encodings = [
        (
            'GT-Adaptive',
            GrowingTreeAdaptiveHuffmanEncoding(257)
        ),
        (
            'Repeat GT-Adaptive',
            PredictionEncoding(lambda: RepeatOracle(0), 257) | GrowingTreeAdaptiveHuffmanEncoding(257)
        ),
        (
            'Markov GT-Adaptive',
            PredictionEncoding(lambda: MarkovOracle(0), 257) | GrowingTreeAdaptiveHuffmanEncoding(257)
        ),
        (
            'M1 GT-Adaptive',
            PredictionEncoding(lambda: MemoryOracle(1, 0), 257) | GrowingTreeAdaptiveHuffmanEncoding(257)
        ),
        (
            'M2 GT-Adaptive',
            PredictionEncoding(lambda: MemoryOracle(2, 0), 257) | GrowingTreeAdaptiveHuffmanEncoding(257)
        ),
        (
            'M5 GT-Adaptive',
            PredictionEncoding(lambda: MemoryOracle(5, 0), 257) | GrowingTreeAdaptiveHuffmanEncoding(257)
        ),
        (
            'FT-Adaptive',
            FullTreeAdaptiveHuffmanEncoding(257)
        ),
    ]

    inputs = [
        b'\0' * 100,
        b'abcdef' * 100,
        b'Fruit flies like a banana'
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


main()