from __future__ import annotations
import typing
from typing import Any, TypeVar, Union, Literal, Generic
from math import ceil, log2
from collections.abc import Iterable, Iterator, Callable
from abc import *
from freqtable import FrequencyTable
from encoding import Encoding
from util import Bit, bits, from_bits, bits_needed, take, rotate_left, append
from tree import Node, Leaf, Branch
from defs import Datum, Data
import cProfile


T = TypeVar('T')
U = TypeVar('U')
V = TypeVar('V')



class MoveToFrontEncoding(Encoding[Data, Data]):
    __nvalues : int

    def __init__(self, nvalues : int):
        self.__nvalues = nvalues

    def encode(self, data : Data) -> Data:
        table = list(range(self.__nvalues))
        for datum in data:
            assert 0 <= datum < self.__nvalues, f'datum={datum}, nvalues={self.__nvalues}'
            index = table.index(datum)
            assert 0 <= index < self.__nvalues
            yield index
            del table[index]
            table.insert(0, datum)

    def decode(self, data : Data) -> Data:
        table = list(range(self.__nvalues))
        for x in data:
            assert 0 <= x < self.__nvalues
            value = table[x]
            assert 0 <= value <= self.__nvalues
            yield value
            del table[x]
            table.insert(0, value)