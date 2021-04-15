import sys
sys.path.append('..')

from eof_encoding import EofEncoding


def test_eof_encoding():
    def check(data, nvalues):
        encoding = EofEncoding(nvalues)
        encoded = encoding.encode(data)
        decoded = list(encoding.decode(encoded))
        assert data == decoded, f'data={data}, decoded={decoded}'

    yield check, [], 4
    yield check, [0], 4
    yield check, [1], 4
    yield check, [1, 1], 4
    yield check, [1, 2, 3, 4, 3, 2, 1], 16
    yield check, list(range(100)), 100