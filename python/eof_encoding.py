from __future__ import annotations
import typing
from util import append
from encoding import Encoding
from defs import Datum, Data



class EofEncoding(Encoding[Data, Data]):
    __nvalues : int

    def __init__(self, nvalues : int):
        self.__nvalues = nvalues

    def encode(self, data : Data) -> Data:
        eof = self.__nvalues
        return append(data, eof)

    def decode(self, data : Data) -> Data:
        eof = self.__nvalues
        values = iter(data)
        while (datum := next(values)) != eof:
            yield datum
