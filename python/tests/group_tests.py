import sys
sys.path.append('..')

from util import group


def test_group():
    def check(expected, xs, group_size):
        assert expected == group(xs, group_size)
    yield check, [[1], [2], [3]], [1, 2, 3], 1
    yield check, [[1, 2], [3]], [1, 2, 3], 2
    yield check, [[1, 2], [3, 4]], [1, 2, 3, 4], 2
    yield check, [[1, 2], [3, 4], [5, 6]], [1, 2, 3, 4, 5, 6], 2

