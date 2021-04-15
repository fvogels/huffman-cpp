from __future__ import annotations
import typing
from typing import Any, TypeVar, Union, Literal, Generic
from collections.abc import Iterable, Iterator, Callable
from abc import *
import struct
from oracles import Oracle
from encoding import Encoding
from defs import Datum, Data



class PredictionEncoding(Encoding[Data, Data]):
    __nvalues : int

    def __init__(self, oracle_factory : Callable[[], Oracle], nvalues : int):
        assert oracle_factory is not None
        assert nvalues > 0
        self.__oracle_factory = oracle_factory
        self.__nvalues = nvalues

    def encode(self, data : Data) -> Data:
        assert data is not None
        oracle = self.__oracle_factory()
        for actual in data:
            assert 0 <= actual < self.__nvalues, f'actual={actual}, nvalues={self.__nvalues}'
            prediction = oracle.predict()
            oracle.tell(actual)
            correction = self.__compute_correction(prediction, actual)
            assert 0 <= correction < self.__nvalues
            yield correction

    def __compute_correction(self, prediction : Datum, actual : Datum) -> Datum:
        result = (actual - prediction) % self.__nvalues
        return result

    def decode(self, corrections : Data) -> Data:
        assert corrections is not None
        oracle = self.__oracle_factory()
        for correction in corrections:
            assert 0 <= correction < self.__nvalues
            prediction = oracle.predict()
            actual = self.__apply_correction(prediction, correction)
            oracle.tell(actual)
            assert 0 <= actual < self.__nvalues
            yield actual

    def __apply_correction(self, prediction : Datum, correction : Datum) -> Datum:
        result = (prediction + correction) % self.__nvalues
        return result
