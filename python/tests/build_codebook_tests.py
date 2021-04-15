import sys
sys.path.append('..')

from huffman_util import build_codebook
from tree import Node, Leaf, Branch


def test_build_codebook():
    def check(expected, tree):
        actual = build_codebook(tree)
        assert expected == actual, repr(actual)
    yield check, { 'a': [0], 'b': [1] }, Branch( Leaf('a'), Leaf('b') )
    yield check, { 'a': [0, 0], 'b': [0, 1], 'c': [1, 0], 'd': [1, 1] }, Branch( Branch( Leaf('a'), Leaf('b') ), Branch( Leaf('c'), Leaf('d') ) )
