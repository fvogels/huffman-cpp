from __future__ import annotations
import typing
from typing import Any, TypeVar, Union, Literal, Generic
from collections.abc import Iterable, Iterator, Callable
import struct
from encoding import Encoding
from defs import Datum, Data
import cProfile


class UnpackEncoding(Encoding[bytes, Data]):
    def encode(self, bs : bytes) -> Data:
        return [ t[0] for t in struct.iter_unpack('B', bs) ]

    def decode(self, ns : Data) -> bytes:
        data = list(ns)
        return struct.pack('B' * len(data), *data)
