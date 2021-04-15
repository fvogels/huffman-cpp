from __future__ import annotations
from typing import Any, TypeVar, Union, Literal, Generic, Type
from collections.abc import Iterable, Iterator, Callable
from abc import *


T = TypeVar('T')


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
        '''
        Initializes the freqtable
        '''
        self.__table = {}

    def __getitem__(self, value : T) -> int:
        '''
        freqtable[x] returns the frequency of x.
        '''
        return self.__table.get(value, 0)

    def increment(self, value : T) -> None:
        '''
        freqtable.increment(x) increments the frequency of x by one.
        '''
        self.__table[value] = self[value] + 1

    @property
    def values(self) -> Iterable[T]:
        '''
        freqtable.values returns a sequence of only the values (i.e., not their frequencies).
        '''
        return self.__table.keys()

    @property
    def items(self) -> Iterable[tuple[T, int]]:
        '''
        freqtable.items returns the frequencies as a sequence of (value, frequency) pairs.
        '''
        return self.__table.items()

    def add_to_domain(self, value : T) -> None:
        if value not in self.__table:
            self.__table[value] = 0

    def __len__(self) -> int:
        return len(self.__table)

    def __str__(self) -> str:
        return str(self.__table)

    def __repr__(self) -> str:
        return repr(self.__table)

    @property
    def most_frequent(self) -> T:
        return max(self.values, key=lambda value: self[value])
