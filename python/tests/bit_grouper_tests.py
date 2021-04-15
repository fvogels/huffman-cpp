import sys
sys.path.append('..')

from bit_grouper import BitGrouperEncoding


def test_bit_grouper():
    def check(bits, group_size):
        encoding = BitGrouperEncoding(group_size)
        encoded = encoding.encode(bits)
        decoded = list(encoding.decode(encoded))[0:len(bits)]
        assert bits == decoded, f'bits={bits}, decoded={decoded}'

    for group_size in [2, 4, 8, 12, 16]:
        yield check, [], group_size
        yield check, [0], group_size
        yield check, [1], group_size
        yield check, [1, 1], group_size
        yield check, [1, 0, 1, 0, 1, 1, 1, 0], group_size
        yield check, [0] * 8 * 10, group_size