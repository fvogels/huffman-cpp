from __future__ import annotations
from typing import Any, TypeVar, Union, Literal, Generic, Callable, Iterable, Iterator
from math import ceil
import struct


def bits(n : int, size : int = 8) -> Bits:
    return [ 1 if c == '1' else 0 for c in bin(n)[2:].rjust(size, '0') ]


def from_bits(bits : Bits) -> int:
    return int(''.join( '1' if b == 1 else '0' for b in bits ), 2)


def shift(xs : list[T], n : int) -> list[T]:
    result = xs[:n]
    del xs[:n]
    return result


T = TypeVar('T')
U = TypeVar('U')

Bit = Union[Literal[0], Literal[1]]
Bits = list[Bit]

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


def datum_to_bits(datum : Datum) -> Bits:
    if isinstance(datum, Eof):
        return [ 0 ]
    else:
        assert isinstance(datum, int), f'datum {repr(datum)} has type {type(datum)} instead of int'
        return [ 1, *bits(datum) ]


def bits_to_datum(bits : Bits) -> Datum:
    if bits.pop(0) == 0:
        return Eof()
    else:
        return from_bits(shift(bits, 8))


def ints_to_datums(ns : Iterable[int]) -> Iterable[Datum]:
    yield from ns
    yield Eof()


def pad(xs : list[T], length : int, padder : T) -> list[T]:
    padding = [ padder ] * (length - len(xs)) # Check what [x] * -5 does
    return [ *xs, *padding ]


def group(xs : list[T], group_size : int) -> list[list[T]]:
    ngroups = ceil(len(xs) / group_size)
    return [ xs[i * group_size : (i+1) * group_size] for i in range(ngroups) ]


def frequencies(xs : Iterable[T]) -> dict[T, int]:
    result : dict[T, int] = {}
    for x in xs:
        old_frequency = result.setdefault(x, 0)
        new_frequency = old_frequency + 1
        result[x] = new_frequency
    return result


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


def build_tree(frequencies : dict[T, int]) -> Node[tuple[T, int]]:
    def weight(node : Node[tuple[T, int]]) -> int:
        if isinstance(node, Leaf):
            return node.datum[1]
        else:
            assert isinstance(node, Branch)
            return weight(node.left) + weight(node.right)

    queue : list[Node[tuple[T, int]]] = [ Leaf((datum, weight)) for datum, weight in frequencies.items() ]
    while len(queue) > 1:
        queue.sort(key=weight)
        branch = Branch(queue.pop(0), queue.pop(0))
        queue.append(branch)
    return queue[0]


def drop_weights(tree : Node[tuple[T, int]]) -> Node[T]:
    return tree.map(lambda pair: pair[0])


def build_codebook(tree : Node[T]) -> dict[T, Bits]:
    def build(node : Node[T], prefix : Bits, book : dict[T, Bits]) -> None:
        if isinstance(node, Leaf):
            book[node.datum] = prefix
        else:
            assert isinstance(node, Branch)
            build(node.left, [*prefix, 0], book)
            build(node.right, [*prefix, 1], book)
    result : dict[T, Bits] = {}
    build(tree, [], result)
    return result


def encode_tree(tree : Node[Datum]) -> Bits:
    result : Bits = []
    def encode(tree : Node[Datum]) -> None:
        nonlocal result
        if isinstance(tree, Leaf):
            result += [ 1, *datum_to_bits(tree.datum) ]
        else:
            assert isinstance(tree, Branch)
            result.append(0)
            encode(tree.left)
            encode(tree.right)
    encode(tree)
    return result


def decode_tree(bits : Bits) -> Node[Datum]:
    if bits.pop(0) == 0:
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


def encode_data(xs : Iterable[T], book : dict[T, Bits]) -> Bits:
    return [ bit for x in xs for bit in book[x] ]


def decode_data(bits : Bits, tree : Node[Union[T, Eof]]) -> list[T]:
    result : list[T] = []
    index = 0
    current_node = tree
    end_reached = False
    while not end_reached:
        if isinstance(current_node, Leaf):
            datum = current_node.datum
            if isinstance(datum, Eof):
                end_reached = True
            else:
                result.append(datum)
                current_node = tree
        else:
            assert isinstance(current_node, Branch)
            if bits[index] == 0:
                current_node = current_node.left
            else:
                current_node = current_node.right
            index += 1
    return result


class Oracle:
    def tell(self, value : int) -> None:
        raise NotImplementedError()

    def predict(self) -> int:
        raise NotImplementedError()


class ZeroOracle(Oracle):
    def tell(self, value : int) -> None:
        pass

    def predict(self) -> int:
        return 0


class RepeatOracle(Oracle):
    last : int

    def __init__(self):
        self.last = 0

    def predict(self) -> int:
        return self.last

    def tell(self, value) -> None:
        self.last = value


def predict(data : Iterable[int], oracle : Oracle) -> Iterable[int]:
    for x in data:
        predicted = oracle.predict()
        oracle.tell(x)
        delta = x - predicted
        if delta < 0:
            delta += 256
        assert x == (predicted + delta) % 256
        yield delta


def unpredict(data : Iterable[int], oracle : Oracle) -> Iterable[int]:
    for delta in data:
        predicted = oracle.predict()
        actual = (predicted + delta) % 256
        oracle.tell(actual)
        yield actual


def huffman_encode(data : bytes) -> bytes:
    freqs : dict[Datum, int] = frequencies(ints_to_datums(data))
    tree : Node[Datum] = drop_weights(build_tree(freqs))
    codes : dict[Datum, Bits] = build_codebook(tree)
    tree_encoding = encode_tree(tree)
    data_encoding = encode_data(ints_to_datums(data), codes)
    encoding_bits : Bits = [ *tree_encoding, *data_encoding ]
    padded_bits : Bits = pad(encoding_bits, ceil(len(encoding_bits) / 8) * 8, 0)
    result : list[int] = [ from_bits(g) for g in group(padded_bits, 8) ]
    return pack(result)


def huffman_decode(data : bytes) -> bytes:
    bs : Bits = [ bit for byte in unpack(data) for bit in bits(byte) ]
    tree : Node[Datum] = decode_tree(bs)
    result : list[int] = decode_data(bs, tree)
    return pack(result)