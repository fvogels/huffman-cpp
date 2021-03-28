from __future__ import annotations
from typing import Any, TypeVar, Union, Literal, Generic
from collections.abc import Iterable, Iterator, Callable
from freqtable import FrequencyTable

T = TypeVar('T')


def last(xs : list[T], n : int) -> list[T]:
    assert n >= 0
    if len(xs) <= n:
        return xs[:]
    else:
        result = xs[len(xs) - n:]
        assert len(result) == n
        return result


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
    __table : dict[T, FrequencyTable[T]]
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
            return self.__table[self.__last].most_frequent
        else:
            return self.__default

    def tell(self, value : T) -> None:
        if self.__first:
            self.__first = False
        else:
            self.__table.setdefault(self.__last, FrequencyTable[T]()).increment(value)
        self.__last = value


class MemoryOracle(Oracle[T]):
    __table : dict[tuple, FrequencyTable[T]]
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
                return self.__table[memory].most_frequent
            else:
                memory = memory[1:]
        return self.__default

    def tell(self, value : T) -> None:
        memory = tuple(self.__memory)
        while len(memory) > 0:
            self.__table.setdefault(memory, FrequencyTable[T]()).increment(value)
            memory = memory[1:]
        self.__memory.append(value)
        self.__memory = last(self.__memory, self.__memory_size)

    def __repr__(self):
        return repr(self.__table)
