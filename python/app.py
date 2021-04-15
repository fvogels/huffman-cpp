from __future__ import annotations
import typing
from typing import Any, TypeVar, Union, Literal, Generic
from defs import Datum, Data
from eof_encoding import EofEncoding
from bit_grouper import BitGrouperEncoding
from huffman import HuffmanEncoding
from unpack import UnpackEncoding
import argparse



encoding = UnpackEncoding() | EofEncoding(256) | HuffmanEncoding(257) | BitGrouperEncoding(8) | ~UnpackEncoding()

def compress(args):
    input_file = args.input
    output_file = args.output
    with open(input_file, 'rb') as file:
        data = file.read()
    compressed = encoding.encode(data)
    with open(output_file, 'wb') as file:
        file.write(compressed)

def decompress(args):
    input_file = args.input
    output_file = args.output
    with open(input_file, 'rb') as file:
        data = file.read()
    decompressed = encoding.decode(data)
    with open(output_file, 'wb') as file:
        file.write(decompressed)


parser = argparse.ArgumentParser()
subparsers = parser.add_subparsers()

subparser = subparsers.add_parser('compress')
subparser.add_argument('input', help='File to be compressed')
subparser.add_argument('output', help='Output file')
subparser.set_defaults(func=compress)

subparser = subparsers.add_parser('decompress')
subparser.add_argument('input', help='File to be decompressed')
subparser.add_argument('output', help='Output file')
subparser.set_defaults(func=decompress)

args = parser.parse_args()
args.func(args)
