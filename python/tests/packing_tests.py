import sys
sys.path.append('..')

from unpack import UnpackEncoding


def test_packing():
    def check(bytes):
        encoding = UnpackEncoding()
        packed = encoding.encode(bytes)
        unpacked = encoding.decode(packed)
        assert bytes == unpacked, repr(unpacked)
    yield check, b""
    yield check, b"\0"
    yield check, b"\0\1"
    yield check, b"\0\1\255"
    yield check, b"abcde456789"
