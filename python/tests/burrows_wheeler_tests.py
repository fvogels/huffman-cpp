import sys
sys.path.append('..')

from burrows_wheeler import BurrowsWheeler


def test_burrows_wheeler():
    def check(data, nvalues):
        encoding = BurrowsWheeler(nvalues)
        transformed = encoding.encode(data)
        untransformed = list(encoding.decode(transformed))
        assert data == untransformed
    yield check, [ 0 ], 2
    yield check, [ 1 ], 2
    yield check, [ 0, 1 ], 2
    yield check, [ 1, 0 ], 2
    yield check, [ 1, 0, 5, 100, 8, 52 ], 256
    yield check, [ 0, 255, 4, 3, 1, 9 ], 256
