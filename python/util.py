from typing import Any, TypeVar, Union, Literal, Generic
from collections.abc import Iterable, Iterator, Callable
from math import ceil, log2


Bit = Union[Literal[0], Literal[1]]

T = TypeVar('T')


def last(xs : list[T], n : int) -> list[T]:
    assert n >= 0
    if len(xs) <= n:
        return xs[:]
    else:
        result = xs[len(xs) - n:]
        assert len(result) == n
        return result


def bits_needed(nvalues : int) -> int:
    return ceil(log2(nvalues))


def bits(n : int, size : int = 8) -> list[Bit]:
    assert size > 0
    return [ 1 if c == '1' else 0 for c in bin(n)[2:].rjust(size, '0') ]


def from_bits(bits : Iterable[Bit]) -> int:
    assert bits is not None
    return int(''.join( '1' if b == 1 else '0' for b in bits ), 2)


def take(xs : Iterator[T], n : int) -> list[T]:
    assert xs is not None
    assert n >= 0
    return [ next(xs) for _ in range(n) ]


def rotate_left(xs : list[T], n : int) -> list[T]:
    assert n >= 0
    return [ *xs[n:], *xs[:n] ]



def group(xs : list[T], group_size : int) -> list[list[T]]:
    ngroups = ceil(len(xs) / group_size)
    return [ xs[i * group_size : (i+1) * group_size] for i in range(ngroups) ]


def pad(xs : list[T], length : int, padder : T) -> list[T]:
    padding = [ padder ] * (length - len(xs)) # Check what [x] * -5 does
    return [ *xs, *padding ]


def append(xs : Iterable[T], x : T) -> Iterable[T]:
    yield from xs
    yield x
