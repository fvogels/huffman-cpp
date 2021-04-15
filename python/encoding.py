from __future__ import annotations
import typing
from typing import Any, TypeVar, Union, Literal, Generic
from math import ceil, log2
from collections.abc import Iterable, Iterator, Callable
from abc import *
from util import Bit, bits, from_bits, bits_needed, take, rotate_left, append


T = TypeVar('T')
U = TypeVar('U')
V = TypeVar('V')


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