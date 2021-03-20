from __future__ import annotations
from typing import Any, TypeVar, Union, Literal, Generic, Callable
from math import ceil


T = TypeVar('T')
Bit = Union[Literal[0], Literal[1]]
Bits = list[Bit]


def pad(xs : list[T], length : int, padder : T) -> list[T]:
    padding = [ padder ] * (length - len(xs)) # Check what [x] * -5 does
    return [ *xs, *padding ]


def group(xs : list[T], group_size : int) -> list[list[T]]:
    ngroups = ceil(len(xs) / group_size)
    return [ xs[i * group_size : (i+1) * group_size] for i in range(ngroups) ]


def bits(n : int, size : int = 8) -> Bits:
    return [ 1 if c == '1' else 0 for c in bin(n)[2:].rjust(size, '0') ]


def from_bits(bits : Bits) -> int:
    return int(''.join( '1' if b == 1 else '0' for b in bits ), 2)


def shift(xs : list[T], n : int) -> list[T]:
    result = xs[:n]
    del xs[:n]
    return result


def count_frequencies(xs : list[T]) -> dict[T, int]:
    result : dict[T, int] = {}
    for x in xs:
        old_frequency = result.setdefault(x, 0)
        new_frequency = old_frequency + 1
        result[x] = new_frequency
    return result
