from __future__ import annotations
from typing import Any, TypeVar, Union, Literal, Generic
from collections.abc import Iterable, Iterator, Callable
from abc import *


T = TypeVar('T')
U = TypeVar('U')


class Node(Generic[T]):
    @abstractmethod
    def map(self : Node[T], func : Callable[[T], U]) -> Node[U]:
        ...

    @abstractmethod
    def __eq__(self : Node[T], other : Any) -> bool:
        ...

    @abstractmethod
    def __repr__(self : Node[T]) -> str:
        ...


class Branch(Node[T]):
    __left : Node[T]
    __right : Node[T]

    def __init__(self : Branch[T], left : Node[T], right : Node[T]):
        self.__left = left
        self.__right = right

    @property
    def left(self : Branch[T]) -> Node[T]:
        return self.__left

    @property
    def right(self : Branch[T]) -> Node[T]:
        return self.__right

    def __eq__(self : Branch[T], other : Any) -> bool:
        return isinstance(other, Branch) and self.left == other.left and self.right == other.right

    def map(self : Branch[T], func : Callable[[T], U]) -> Branch[U]:
        # (for some reason, mypy fails to typecheck this line)
        left_child : Node[U] = self.left.map(func) # type: ignore
        right_child : Node[U] = self.right.map(func)
        return Branch(left_child, right_child)

    def __repr__(self : Branch[T]) -> str:
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
