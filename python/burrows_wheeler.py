from __future__ import annotations
import typing
from typing import Any, TypeVar, Union, Literal, Generic
from math import ceil, log2
from collections.abc import Iterable, Iterator, Callable
from abc import *
from freqtable import FrequencyTable
from util import rotate_left
from encoding import Encoding
from defs import Datum, Data
import cProfile


class BurrowsWheeler(Encoding[Data, Data]):
    __nvalues : int

    def __init__(self, nvalues : int):
        assert nvalues > 0
        self.__nvalues = nvalues

    def encode(self, data : Data) -> Data:
        xs = [ *data, self.__nvalues ]
        rotations = [ rotate_left(xs, i) for i in range(len(xs)) ]
        rotations.sort()
        return (rotation[-1] for rotation in rotations)

    def decode(self, data : Data) -> Data:
        xs = list(data)
        assert all(0 <= x < self.__nvalues+1 for x in xs)
        table : list[list[Datum]] = [ [] for _ in range(len(xs)) ]
        for _ in range(len(xs)):
            for row, datum in zip(table, xs):
                row.insert(0, datum)
            table.sort()
        result : Data = next(row for row in table if row[-1] == self.__nvalues)[:-1]
        assert all(0 <= x < self.__nvalues for x in result)
        return result