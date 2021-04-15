from __future__ import annotations
import typing
from typing import Any, TypeVar, Union, Literal, Generic
from collections.abc import Iterable, Iterator
from encoding import Encoding
from defs import Datum, Data
from util import Bit, bits, from_bits, bits_needed


class BitGrouperEncoding(Encoding[Iterable[Bit], Data]):
    __nbits : int

    def __init__(self, nbits : int):
        assert nbits > 0
        self.__nbits = nbits

    def encode(self, bits : Iterable[Bit]) -> Data:
        bitcount = 0
        acc = 0
        for bit in bits:
            assert bit == 0 or bit == 1
            acc = acc * 2 + bit
            bitcount += 1
            if bitcount == self.__nbits:
                yield acc
                acc = 0
                bitcount = 0
        while bitcount < self.__nbits:
            acc *= 2
            bitcount += 1
        yield acc

    def decode(self, data : Data) -> Iterable[Bit]:
        for byte in data:
            for bit in bits(byte, self.__nbits):
                yield bit
