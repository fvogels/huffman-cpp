from __future__ import annotations
from typing import Any, TypeVar, Union, Literal, Generic
from math import ceil, log2
from collections.abc import Iterable, Iterator, Callable
from abc import *
import struct




T = TypeVar('T')
U = TypeVar('U')
V = TypeVar('V')

Bit = Union[Literal[0], Literal[1]]


class FrequencyTable(Generic[T]):
    __table : dict[T, int]

    @staticmethod
    def count_from_iterable(values : Iterable[T]) -> FrequencyTable[T]:
        assert values is not None
        result = FrequencyTable[T]()
        for value in values:
            result.increment(value)
        return result

    @staticmethod
    def with_domain(values : Iterable[T]) -> FrequencyTable[T]:
        assert values is not None
        result = FrequencyTable[T]()
        for value in values:
            result.add_to_domain(value)
        return result

    def __init__(self):
        self.__table = {}

    def __getitem__(self, value : T) -> int:
        return self.__table.get(value, 0)

    def add_to_domain(self, value : T) -> None:
        if value not in self.__table:
            self.__table[value] = 0

    def increment(self, value : T) -> None:
        self.__table[value] = self[value] + 1

    @property
    def values(self) -> Iterable[T]:
        return self.__table.keys()

    @property
    def items(self) -> Iterable[tuple[T, int]]:
        return self.__table.items()

    def __len__(self) -> int:
        return len(self.__table)

    def __str__(self) -> str:
        return str(self.__table)

    def __repr__(self) -> str:
        return repr(self.__table)


def bits_needed(nvalues : int) -> int:
    return ceil(log2(nvalues))


def bits(n : int, size : int = 8) -> list[Bit]:
    assert size > 0
    return [ 1 if c == '1' else 0 for c in bin(n)[2:].rjust(size, '0') ]


def from_bits(bits : Iterable[Bit]) -> int:
    assert bits is not None
    return int(''.join( '1' if b == 1 else '0' for b in bits ), 2)


def take(xs : Iterator[T], n : int) -> list[T]:
    assert xs is not None
    assert n >= 0
    return [ next(xs) for _ in range(n) ]


def rotate_left(xs : list[T], n : int) -> list[T]:
    assert n >= 0
    return [ *xs[n:], *xs[:n] ]


def last(xs : list[T], n : int) -> list[T]:
    assert n >= 0
    if len(xs) <= n:
        return xs[:]
    else:
        result = xs[len(xs) - n:]
        assert len(result) == n
        return result

def group(xs : list[T], group_size : int) -> list[list[T]]:
    ngroups = ceil(len(xs) / group_size)
    return [ xs[i * group_size : (i+1) * group_size] for i in range(ngroups) ]


def pad(xs : list[T], length : int, padder : T) -> list[T]:
    padding = [ padder ] * (length - len(xs)) # Check what [x] * -5 does
    return [ *xs, *padding ]


def append(xs : Iterable[T], x : T) -> Iterable[T]:
    yield from xs
    yield x

Datum = int

class Data:
    __values : Iterable[Datum]
    __nvalues : int

    def __init__(self, values : Iterable[Datum], nvalues : int):
        self.__values = values
        self.__nvalues = nvalues

    @property
    def values(self) -> Iterable[Datum]:
        for n in self.__values:
            assert 0 <= n < self.__nvalues, f'n={n}, nvalues={self.__nvalues}'
            yield n

    @property
    def nvalues(self) -> int:
        return self.__nvalues

    def __eq__(self, other : Any) -> bool:
        return list(self.values) == list(other.values) and self.nvalues == other.nvalues

    def __repr__(self) -> str:
        return repr(list(self.values))


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
        left = self.left.map(func)
        right = self.right.map(func)
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


def pack(code : list[int]) -> bytes:
    return struct.pack('B' * len(code), *code)


def unpack(bs : bytes) -> list[int]:
    return [ t[0] for t in struct.iter_unpack('B', bs) ]


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


class Oracle(Generic[T]):
    def tell(self, value : T) -> None:
        raise NotImplementedError()

    def predict(self) -> T:
        raise NotImplementedError()


class ConstantOracle(Oracle[T]):
    def __init__(self, constant : T):
        self.__constant = constant

    def tell(self, value : T) -> None:
        pass

    def predict(self) -> T:
        return self.__constant


class RepeatOracle(Oracle[T]):
    last : T

    def __init__(self, initial : T):
        self.last = initial

    def predict(self) -> T:
        return self.last

    def tell(self, value : T) -> None:
        self.last = value


class MarkovOracle(Oracle[T]):
    __table : dict[T, dict[T, int]]
    __last : T
    __first : bool
    __default : T

    def __init__(self, default : T):
        self.__table = {}
        self.__last = default
        self.__first = True
        self.__default = default

    def predict(self) -> T:
        if not self.__first and self.__last in self.__table:
            d = self.__table[self.__last]
            return max(d.keys(), key=lambda x: d[x])
        else:
            return self.__default

    def tell(self, value : T) -> None:
        if self.__first:
            self.__first = False
        else:
            d = self.__table.setdefault(self.__last, {})
            n = d.get(value, 0)
            d[value] = n + 1
        self.__last = value


class MemoryOracle(Oracle[T]):
    __table : dict[tuple, dict[T, int]]
    __memory : list[T]
    __memory_size : int
    __default : T

    def __init__(self, memory_size : int, default : T):
        self.__table = {}
        self.__memory = []
        self.__memory_size = memory_size
        self.__default = default

    def predict(self) -> T:
        memory : tuple = tuple(self.__memory)
        while len(memory) > 0:
            if memory in self.__table:
                d = self.__table[memory]
                return max(d.keys(), key=lambda x: d[x])
            else:
                memory = memory[1:]
        return self.__default

    def tell(self, value : T) -> None:
        def inc(memory : tuple):
            d = self.__table.setdefault(memory, {})
            n = d.get(value, 0)
            d[value] = n + 1
        memory = tuple(self.__memory)
        while len(memory) > 0:
            inc(memory)
            memory = memory[1:]
        self.__memory.append(value)
        self.__memory = last(self.__memory, self.__memory_size)

    def __repr__(self):
        return repr(self.__table)


class Encoding(Generic[T,U]):
    def encode(self, x : T) -> U:
        raise NotImplementedError()

    def decode(self, x : U) -> T:
        raise NotImplementedError()

    def __or__(self, other : Encoding[U,V]) -> Encoding[T,V]:
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


class DataEncoding(Encoding[Iterable[int], Data]):
    __nvalues : int

    def __init__(self, nvalues : int):
        assert 0 < nvalues
        self.__nvalues = nvalues

    def encode(self, ns : Iterable[int]) -> Data:
        return Data(ns, self.__nvalues)

    def decode(self, data : Data) -> Iterable[int]:
        return data.values


class PredictionEncoding(Encoding[Data, Data]):
    def __init__(self, oracle_factory : Callable[[], Oracle]):
        assert oracle_factory is not None
        self.__oracle_factory = oracle_factory

    def encode(self, data : Data) -> Data:
        assert data is not None
        def enc() -> Iterable[Datum]:
            oracle = self.__oracle_factory()
            for actual in data.values:
                prediction = oracle.predict()
                oracle.tell(actual)
                correction = self.__compute_correction(prediction, actual, data.nvalues)
                yield correction
        return Data(enc(), data.nvalues)

    def __compute_correction(self, prediction : Datum, actual : Datum, nvalues : int) -> Datum:
        result = (actual - prediction) % nvalues
        return result

    def decode(self, corrections : Data) -> Data:
        assert corrections is not None
        def dec() -> Iterable[Datum]:
            oracle = self.__oracle_factory()
            for correction in corrections.values:
                prediction = oracle.predict()
                actual = self.__apply_correction(prediction, correction, corrections.nvalues)
                oracle.tell(actual)
                yield actual
        return Data(dec(), corrections.nvalues)

    def __apply_correction(self, prediction : Datum, correction : Datum, nvalues : int) -> Datum:
        result = (prediction + correction) % nvalues
        return result


class BurrowsWheeler(Encoding[Data, Data]):
    def encode(self, data : Data) -> Data:
        xs = [ *data.values, data.nvalues ]
        rotations = [ rotate_left(xs, i) for i in range(len(xs)) ]
        rotations.sort()
        return Data((rotation[-1] for rotation in rotations), data.nvalues + 1)

    def decode(self, data : Data) -> Data:
        xs = list(data.values)
        table : list[list[Datum]] = [ [] for _ in range(len(xs)) ]
        for _ in range(len(xs)):
            for row, datum in zip(table, xs):
                row.insert(0, datum)
            table.sort()
        result = next(row for row in table if row[-1] is data.nvalues-1)[:-1]
        return Data(result, data.nvalues - 1)


class HuffmanEncoding(Encoding[Data, Iterable[Bit]]):
    __tree_encoding : TreeEncoding
    __nvalues : int

    def __init__(self, nvalues : int):
        assert 0 < nvalues
        self.__nvalues = nvalues
        self.__tree_encoding = TreeEncoding(bits_needed(nvalues))

    def encode(self, data : Data) -> Iterable[Bit]:
        assert data.nvalues == self.__nvalues
        values = list(data.values)
        frequencies : FrequencyTable[Datum] = FrequencyTable.count_from_iterable(values)
        tree : Node[Datum] = build_tree(frequencies)
        codes : dict[Datum, list[Bit]] = build_codebook(tree)
        yield from self.__tree_encoding.encode(tree)
        yield from encode_data(values, codes)

    def decode(self, bits : Iterable[Bit]) -> Data:
        iterator = iter(bits)
        tree : Node[Datum] = self.__tree_encoding.decode(iterator)
        decoded = decode_data(iterator, tree)
        return Data(decoded, self.__nvalues)


class GrowingTreeAdaptiveHuffmanEncoding(Encoding[Data, Iterable[Bit]]):
    __nvalues : int

    def __init__(self, nvalues : int):
        assert nvalues > 1
        self.__nvalues = nvalues

    def encode(self, data : Data) -> Iterable[Bit]:
        not_yet_transmitted = data.nvalues
        frequencies : FrequencyTable[Datum] = FrequencyTable.with_domain([ not_yet_transmitted ])
        bn = bits_needed(self.__nvalues)

        for datum in data.values:
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
        def dec():
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
        return Data(dec(), self.__nvalues)


class FullTreeAdaptiveHuffmanEncoding(Encoding[Data, Iterable[Bit]]):
    __nvalues : int

    def __init__(self, nvalues : int):
        assert nvalues > 1
        self.__nvalues = nvalues

    def encode(self, data : Data) -> Iterable[Bit]:
        frequencies : FrequencyTable[Datum] = FrequencyTable.with_domain(range(self.__nvalues))

        for datum in data.values:
            tree : Node[Datum] = build_tree(frequencies)
            codes : dict[Datum, list[Bit]] = build_codebook(tree)
            yield from codes[datum]
            frequencies.increment(datum)

    def decode(self, bits : Iterable[Bit]) -> Data:
        def dec():
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
        return Data(dec(), self.__nvalues)

class MoveToFrontEncoding(Encoding[Data, Data]):
    def encode(self, data : Data) -> Data:
        def enc() -> Iterable[Datum]:
            table = list(range(data.nvalues))
            for x in data.values:
                index = table.index(x)
                yield index
                del table[index]
                table.insert(0, x)
        return Data(enc(), data.nvalues)

    def decode(self, data : Data) -> Data:
        def dec():
            table = list(range(data.nvalues))
            for x in data.values:
                value = table[x]
                yield value
                del table[x]
                table.insert(0, value)
        return Data(dec(), data.nvalues)


class BitGrouperEncoding(Encoding[Iterable[Bit], Data]):
    __nbits : int

    def __init__(self, nbits : int):
        assert nbits > 0
        self.__nbits = nbits

    def encode(self, bits : Iterable[Bit]) -> Data:
        def enc():
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
        return Data(enc(), 2 ** self.__nbits)

    def decode(self, data : Data) -> Iterable[Bit]:
        assert bits_needed(data.nvalues) == self.__nbits
        for byte in data.values:
            for bit in bits(byte, self.__nbits):
                yield bit


class EofEncoding(Encoding[Data, Data]):
    __nvalues : int

    def __init__(self, nvalues : int):
        self.__nvalues = nvalues

    def encode(self, data : Data) -> Data:
        assert data.nvalues == self.__nvalues
        eof = data.nvalues
        return Data(append(data.values, eof), data.nvalues + 1)

    def decode(self, data : Data) -> Data:
        assert data.nvalues == self.__nvalues + 1
        eof = self.__nvalues
        def dec():
            values = data.values
            while (datum := next(values)) != eof:
                yield datum
        return Data(dec(), self.__nvalues)
