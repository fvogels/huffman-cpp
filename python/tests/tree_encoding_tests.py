import sys
sys.path.append('..')

from huffman import TreeEncoding, HuffmanEncoding
from tree import Node, Leaf, Branch


def test_tree_encoding():
    def check(tree):
        encoding = TreeEncoding(4)
        encoded = encoding.encode(tree)
        decoded = encoding.decode(encoded)
        assert tree == decoded
    def l(value):
        return Leaf(value)
    def b(left, right):
        return Branch(left, right)
    yield check, l(0)
    yield check, l(1)
    yield check, l(5)
    yield check, l(15)
    yield check, b(l(0), l(1))
    yield check, b(l(0), l(4))
    yield check, b(b(l(0), l(1)), l(9))
