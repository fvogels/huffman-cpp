from __future__ import annotations
from typing import Any, TypeVar, Union, Literal, Generic, Callable
from math import ceil


T = TypeVar('T')
U = TypeVar('U')

Bit = Union[Literal[0], Literal[1]]
Bits = list[Bit]


def pad(xs : list[T], length : int, padder : T) -> list[T]:
    padding = [ padder ] * (length - len(xs)) # Check what [x] * -5 does
    return [ *xs, *padding ]


def group(xs : list[T], group_size : int) -> list[list[T]]:
    ngroups = ceil(len(xs) / group_size)
    return [ xs[i * group_size : (i+1) * group_size] for i in range(ngroups) ]


def bits(n : int, size : int = 8) -> Bits:
    return [ 1 if c == '1' else 0 for c in bin(n)[2:].rjust(size, '0') ]


def from_bits(bits : Bits) -> int:
    return int(''.join( '1' if b == 1 else '0' for b in bits ), 2)


def shift(xs : list[T], n : int) -> list[T]:
    result = xs[:n]
    del xs[:n]
    return result


def frequencies(xs : list[T]) -> dict[T, int]:
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
    def __init__(self, left : Node[T], right : Node[T]):
        self.left = left
        self.right = right

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Branch) and self.left == other.left and self.right == other.right

    def map(self, func : Callable[[T], U]) -> Branch[U]:
        left = self.left.map(func)
        right = self.right.map(func)
        return Branch(left, right)


class Leaf(Node[T]):
    def __init__(self, datum : T):
        self.datum = datum

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Leaf) and self.datum == other.datum

    def map(self, func : Callable[[T], U]) -> Leaf[U]:
        return Leaf(func(self.datum))


def build_tree(frequencies : dict[T, int]) -> Node[tuple[T, int]]:
    def weight(node : Node[tuple[T, int]]) -> int:
        if isinstance(node, Leaf):
            return node.datum[1]
        elif isinstance(node, Branch):
            return weight(node.left) + weight(node.right)
        else:
            raise NotImplementedError()

    queue : list[Node[tuple[T, int]]] = [ Leaf((datum, weight)) for datum, weight in frequencies.items() ]
    while len(queue) > 1:
        queue.sort(key=weight)
        branch = Branch(queue.pop(0), queue.pop(0))
        queue.append(branch)
    return queue[0]


def drop_weights(tree : Node[tuple[T, int]]) -> Node[T]:
    return tree.map(lambda pair: pair[0])
