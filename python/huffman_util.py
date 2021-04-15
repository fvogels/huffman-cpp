from __future__ import annotations
import typing
from typing import Any, TypeVar, Union, Literal, Generic
from collections.abc import Iterable, Iterator, Callable
from abc import *
from freqtable import FrequencyTable
from util import Bit, bits, from_bits, bits_needed, take, append
from encoding import Encoding
from tree import Node, Leaf, Branch
from defs import Datum, Data


T = TypeVar('T')
U = TypeVar('U')



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
