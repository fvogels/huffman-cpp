import sys
sys.path.append('..')

from move_to_front import MoveToFrontEncoding
from prediction import PredictionEncoding
from unpack import UnpackEncoding
from oracles import MemoryOracle, MarkovOracle, ConstantOracle, RepeatOracle


def test_encoding_inverter():
    def check(encoding, data):
        e = encoding | ~encoding
        encoded = e.encode(data)
        decoded = list(e.decode(encoded))
        assert data == decoded

    yield check, PredictionEncoding(lambda: MemoryOracle(2, 0), 10), []
    yield check, PredictionEncoding(lambda: MemoryOracle(2, 0), 10), [1, 1, 1, 1, 1]
    yield check, PredictionEncoding(lambda: MemoryOracle(2, 0), 10), [1, 2, 1, 3, 1, 4, 1, 5]
    yield check, MoveToFrontEncoding(10), []
    yield check, MoveToFrontEncoding(10), [0]
    yield check, MoveToFrontEncoding(10), [0, 0, 0, 0, 0]
    yield check, MoveToFrontEncoding(10), [1, 1, 1, 1, 1]
