import sys
sys.path.append('..')

from move_to_front import MoveToFrontEncoding



def test_move_to_front():
    encoding = MoveToFrontEncoding(256)
    def check(data):
        encoded = encoding.encode(data)
        decoded = list(encoding.decode(encoded))
        assert data == decoded, f'{data} != {decoded}'
    yield check, []
    yield check, [0]
    yield check, [5]
    yield check, [2, 9]
    yield check, [5, 2, 9, 100]
    yield check, [8, 7, 5, 4, 3, 2, 1, 5, 7, 8, 9, 5, 6]
