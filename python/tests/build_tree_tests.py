import sys
sys.path.append('..')

from huffman_util import build_tree
from tree import Node, Leaf, Branch
from freqtable import FrequencyTable


def test_build_tree():
    def check(expected, d):
        frequencies : FrequencyTable[str] = FrequencyTable[str]()
        for value, n in d.items():
            for _ in range(n):
                frequencies.increment(value)
        assert expected == build_tree(frequencies)

    yield check, Leaf('a'), { 'a': 1 }
    yield check, Branch( Leaf('a'), Leaf('b') ), { 'a': 1, 'b': 2 }
    yield check, Branch( Leaf('b'), Leaf('a') ), { 'a': 2, 'b': 1 }
