from __future__ import annotations
from typing import Any, TypeVar, Union, Literal, Generic, Callable


T = TypeVar('T')


def pad(xs : list[T], length : int, padder : T) -> list[T]:
    padding = [ padder ] * (length - len(xs)) # Check what [x] * -5 does
    return [ *xs, *padding ]
