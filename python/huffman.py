from __future__ import annotations
from typing import Any, TypeVar, Union, Literal, Generic, Callable, Iterable, Iterator
from math import ceil
import struct


T = TypeVar('T')
U = TypeVar('U')
V = TypeVar('V')

Bit = Union[Literal[0], Literal[1]]


class FrequencyTable(Generic[T]):
    __table : dict[T, int]

    @staticmethod
    def from_iterable(values : Iterable[T]) -> FrequencyTable[T]:
        result = FrequencyTable[T]()
        for value in values:
            result.increment(value)
        return result

    def __init__(self):
        self.__table = {}

    def __getitem__(self, value : T) -> int:
        return self.__table.get(value, 0)

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


def bits(n : int, size : int = 8) -> list[Bit]:
    return [ 1 if c == '1' else 0 for c in bin(n)[2:].rjust(size, '0') ]


def from_bits(bits : Iterable[Bit]) -> int:
    return int(''.join( '1' if b == 1 else '0' for b in bits ), 2)


def shift(xs : list[T], n : int) -> list[T]:
    result = xs[:n]
    del xs[:n]
    return result


def take(xs : Iterator[T], n : int) -> list[T]:
    return [ next(xs) for _ in range(n) ]


def rotate_left(xs : list[T], n : int) -> list[T]:
    return [ *xs[n:], *xs[:n] ]


def last(xs : list[T], n : int) -> list[T]:
    if len(xs) <= n:
        return xs[:]
    else:
        result = xs[len(xs) - n:]
        assert len(result) == n
        return result


class Eof:
    def __str__(self):
        return 'EOF'

    def __repr__(self):
        return 'EOF'

    def __hash__(self):
        return 0

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Eof)


Datum = Union[int, Eof]


def datum_to_bits(datum : Datum) -> Iterable[Bit]:
    if isinstance(datum, Eof):
        yield 0
    else:
        assert isinstance(datum, int), f'datum {repr(datum)} has type {type(datum)} instead of int'
        yield 1
        yield from bits(datum)


def bits_to_datum(bits : Iterator[Bit]) -> Datum:
    if next(bits) == 0:
        return Eof()
    else:
        return from_bits(take(bits, 8))


def ints_to_datums(ns : Iterable[int]) -> Iterable[Datum]:
    yield from ns
    yield Eof()


def pad(xs : list[T], length : int, padder : T) -> list[T]:
    padding = [ padder ] * (length - len(xs)) # Check what [x] * -5 does
    return [ *xs, *padding ]


def group(xs : list[T], group_size : int) -> list[list[T]]:
    ngroups = ceil(len(xs) / group_size)
    return [ xs[i * group_size : (i+1) * group_size] for i in range(ngroups) ]


class Node(Generic[T]):
    def map(self, func : Callable[[T], U]) -> Node[U]:
        raise NotImplementedError()


class Branch(Node[T]):
    left : Node[T]
    right : Node[T]

    def __init__(self, left : Node[T], right : Node[T]):
        self.left = left
        self.right = right

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Branch) and self.left == other.left and self.right == other.right

    def map(self, func : Callable[[T], U]) -> Branch[U]:
        left = self.left.map(func)
        right = self.right.map(func)
        return Branch(left, right)

    def __repr__(self):
        return f"B[{self.left}|{self.right}]"


class Leaf(Node[T]):
    datum : T

    def __init__(self, datum : T):
        self.datum = datum

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Leaf) and self.datum == other.datum

    def map(self, func : Callable[[T], U]) -> Leaf[U]:
        return Leaf(func(self.datum))

    def __repr__(self):
        return f"L[{repr(self.datum)}]"


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


def encode_tree(tree : Node[Datum]) -> Iterable[Bit]:
    def encode(tree : Node[Datum]) -> Iterable[Bit]:
        if isinstance(tree, Leaf):
            yield 1
            yield from datum_to_bits(tree.datum)
        else:
            assert isinstance(tree, Branch)
            yield 0
            yield from encode(tree.left)
            yield from encode(tree.right)
    yield from encode(tree)


def decode_tree(bits : Iterator[Bit]) -> Node[Datum]:
    if next(bits) == 0:
        left = decode_tree(bits)
        right = decode_tree(bits)
        return Branch(left, right)
    else:
        datum = bits_to_datum(bits)
        return Leaf(datum)


def pack(code : list[int]) -> bytes:
    return struct.pack('B' * len(code), *code)


def unpack(bs : bytes) -> list[int]:
    return [ t[0] for t in struct.iter_unpack('B', bs) ]


def encode_data(xs : Iterable[T], book : dict[T, list[Bit]]) -> list[Bit]:
    return [ bit for x in xs for bit in book[x] ]


def decode_data(bits : Iterator[Bit], tree : Node[Union[Datum]]) -> Iterable[Datum]:
    current_node = tree
    end_reached = False
    while not end_reached:
        if isinstance(current_node, Leaf):
            datum = current_node.datum
            if isinstance(datum, Eof):
                end_reached = True
            else:
                current_node = tree
            yield datum
        else:
            assert isinstance(current_node, Branch)
            if next(bits) == 0:
                current_node = current_node.left
            else:
                current_node = current_node.right


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
    def encode(self, xs : Iterable[T]) -> Iterable[U]:
        raise NotImplementedError()

    def decode(self, xs : Iterable[U]) -> Iterable[T]:
        raise NotImplementedError()

    def __or__(self, other : Encoding[U,V]) -> Encoding[T,V]:
        return EncodingCombinator(self, other)


class EncodingCombinator(Encoding[T, V]):
    def __init__(self, left : Encoding[T, U], right : Encoding[U, V]):
        self.__left = left
        self.__right = right

    def encode(self, xs : Iterable[T]) -> Iterable[V]:
        return self.__right.encode(self.__left.encode(xs))

    def decode(self, xs : Iterable[V]) -> Iterable[T]:
        return self.__left.decode(self.__right.decode(xs))


class DatumEncoding(Encoding[int, Datum]):
    def encode(self, xs : Iterable[int]) -> Iterable[Datum]:
        yield from xs
        yield Eof()

    def decode(self, xs : Iterable[Datum]) -> Iterable[int]:
        end_reached = False
        for x in xs:
            if isinstance(x, Eof):
                assert not end_reached
                end_reached = True
            else:
                yield x

class PredictionEncoding(Encoding[int, int]):
    def __init__(self, oracle_factory : Callable[[], Oracle]):
        assert oracle_factory is not None
        self.__oracle_factory = oracle_factory

    def encode(self, data : Iterable[int]) -> Iterable[int]:
        assert data is not None
        oracle = self.__oracle_factory()
        for actual in data:
            prediction = oracle.predict()
            oracle.tell(actual)
            correction = self.__compute_correction(prediction, actual)
            yield correction

    def __compute_correction(self, prediction : int, actual : int) -> int:
        assert 0 <= prediction <= 255
        assert 0 <= actual <= 255
        result = (actual - prediction) % 256
        assert 0 <= result <= 255
        return result

    def decode(self, corrections : Iterable[int]) -> Iterable[int]:
        assert corrections is not None
        oracle = self.__oracle_factory()
        for correction in corrections:
            prediction = oracle.predict()
            actual = self.__apply_correction(prediction, correction)
            oracle.tell(actual)
            yield actual

    def __apply_correction(self, prediction : int, correction : int) -> int:
        assert 0 <= prediction <= 255
        assert 0 <= correction <= 255
        result = (prediction + correction) % 256
        assert 0 <= result <= 255
        return result


class BurrowsWheeler(Encoding[Datum, Datum]):
    def encode(self, data : Iterable[Datum]) -> Iterable[Datum]:
        def key(datum : Datum) -> int:
            if isinstance(datum, Eof):
                return -1
            else:
                return datum
        xs = list(data)
        rotations = [ rotate_left(xs, i) for i in range(len(xs)) ]
        rotations.sort(key=lambda ds: list(map(key, ds)))
        return (rotation[-1] for rotation in rotations)

    def decode(self, data : Iterable[Datum]) -> Iterable[Datum]:
        def key(datum : Datum) -> int:
            if isinstance(datum, Eof):
                return -1
            else:
                return datum
        xs = list(data)
        table : list[list[Datum]] = [ [] for _ in range(len(xs)) ]
        for _ in range(len(xs)):
            for row, datum in zip(table, xs):
                row.insert(0, datum)
            table.sort(key=lambda ds: list(map(key, ds)))
        return next(row for row in table if isinstance(row[-1], Eof))


class HuffmanEncoding(Encoding[Datum, Bit]):
    def encode(self, data : Iterable[Datum]) -> Iterable[Bit]:
        frequencies : FrequencyTable[Datum] = FrequencyTable.from_iterable(data)
        tree : Node[Datum] = build_tree(frequencies)
        codes : dict[Datum, list[Bit]] = build_codebook(tree)
        yield from encode_tree(tree)
        yield from encode_data(data, codes)

    def decode(self, bits : Iterable[Bit]) -> Iterable[Datum]:
        iterator = iter(bits)
        tree : Node[Datum] = decode_tree(iterator)
        return decode_data(iterator, tree)


class MoveToFrontEncoding(Encoding[int, int]):
    def encode(self, data : Iterable[int]) -> Iterable[int]:
        table = list(range(2**8))
        for x in data:
            assert 0 <= x <= 255
            index = table.index(x)
            yield index
            del table[index]
            table.insert(0, x)

    def decode(self, data : Iterable[int]) -> Iterable[int]:
        table = list(range(2**8))
        for x in data:
            assert 0 <= x <= 255
            value = table[x]
            yield value
            del table[x]
            table.insert(0, value)


# Introduce type Byte, is_byte

# with open('huffman.py', 'rb') as file:
#     data : list[int] = unpack(file.read())

# # data : list[int] = [1,2,3,4,5] * 1000

# # data = burrows_wheeler_transform(data)
# # oracle = ConstantOracle[int](0)
# # oracle = RepeatOracle[int](0)
# # oracle = MarkovOracle[int](0)
# oracle = MemoryOracle[int](2, 0)
# with_prediction = list(predict(data, oracle))
# # print(with_prediction)
# # print(oracle)
# print(len(list(data)))
# print(len(list(huffman_encode(data))))
# print(len(list(huffman_encode(with_prediction))))
