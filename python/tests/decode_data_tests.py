import sys
sys.path.append('..')

from huffman_util import decode_data
from tree import Node, Leaf, Branch


def test_decode_data():
    def check(expected, data, tree):
        expected = [*expected]
        actual = list(decode_data(iter(data), tree))
        assert expected == actual, f'actual={repr(actual)}, expected={expected}'
    def B(left, right):
        return Branch(left, right)

    a = Leaf('a')
    b = Leaf('b')
    c = Leaf('c')

    yield check, 'a', [0], B(a, b)
    yield check, 'aa', [0, 0], B(a, b)
    yield check, 'ab', [0, 1], B(a, b)
    yield check, 'a', [0, 0], B(B(a, b), c)
    yield check, 'b', [0, 1], B(B(a, b), c)
    yield check, 'c', [1], B(B(a, b), c)
    yield check, 'abc', [0,0,0,1,1], B(B(a, b), c)
