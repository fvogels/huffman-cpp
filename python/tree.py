from __future__ import annotations
from typing import Any, TypeVar, Union, Literal, Generic
from collections.abc import Iterable, Iterator, Callable
from abc import *


T = TypeVar('T')
U = TypeVar('U')


class Node(Generic[T]):
    @abstractmethod
    def __eq__(self : Node[T], other : Any) -> bool:
        ...

    @abstractmethod
    def __repr__(self : Node[T]) -> str:
        ...

    @abstractmethod
    def map(self : Node[T], func : Callable[[T], U]) -> Node[U]:
        '''
        node.map(f) applies f on the leaves' data, thereby constructing a new tree where the leaves carry f(datum).
        '''
        ...


class Branch(Node[T]):
    '''
    A branch has two child nodes.
    '''

    __left : Node[T]
    __right : Node[T]

    def __init__(self : Branch[T], left : Node[T], right : Node[T]):
        self.__left = left
        self.__right = right

    @property
    def left(self : Branch[T]) -> Node[T]:
        '''
        branch.left returns the left child of the branch.
        '''
        return self.__left

    @property
    def right(self : Branch[T]) -> Node[T]:
        '''
        branch.left returns the right child of the branch.
        '''
        return self.__right

    def __eq__(self : Branch[T], other : Any) -> bool:
        '''
        branch == x is true if x is also a branch and the children are equal (left == left and right == right).
        '''
        return isinstance(other, Branch) and self.left == other.left and self.right == other.right

    def map(self : Branch[T], func : Callable[[T], U]) -> Branch[U]:
        '''
        Calls map on both children and constructs a new branch with the results.
        '''
        # (for some reason, mypy fails to typecheck this line)
        left_child : Node[U] = self.left.map(func) # type: ignore
        right_child : Node[U] = self.right.map(func)
        return Branch(left_child, right_child)

    def __repr__(self : Branch[T]) -> str:
        '''
        repr(branch) return a string representation of the branch.
        '''
        return f"B[{self.left}|{self.right}]"


class Leaf(Node[T]):
    '''
    A leaf has a piece of data (called datum) of type T associated with it.
    '''

    datum : T

    def __init__(self, datum : T):
        self.datum = datum

    def __eq__(self, other : Any) -> bool:
        '''
        leaf == x is true if x is a leaf and their data are equal
        '''
        return isinstance(other, Leaf) and self.datum == other.datum

    def map(self, func : Callable[[T], U]) -> Leaf[U]:
        '''
        Applies func to the leaf's datum and constructs a new leaf with it.
        '''
        return Leaf(func(self.datum))

    def __repr__(self) -> str:
        '''
        Returns string representation of leaf.
        '''
        return f"L[{repr(self.datum)}]"
