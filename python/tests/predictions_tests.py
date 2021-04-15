import sys
sys.path.append('..')

from prediction import PredictionEncoding
from oracles import MemoryOracle, MarkovOracle, ConstantOracle, RepeatOracle


def test_predictions():
    def check(data, oracle_factory):
        encoding = PredictionEncoding(oracle_factory, 256)
        encoded = encoding.encode(data)
        restored = list(encoding.decode(encoded))
        assert restored == data, f'{restored} != {data}'

    oracles = [
        lambda: ConstantOracle(0),
        lambda: RepeatOracle(0),
        lambda: MarkovOracle(0),
        lambda: MemoryOracle(1, 0),
        lambda: MemoryOracle(2, 0),
        lambda: MemoryOracle(3, 0),
    ]

    for oracle in oracles:
        yield check, [0], oracle
        yield check, [0, 1], oracle
        yield check, [0, 1, 2, 3, 4], oracle
        yield check, [5, 1, 2, 9, 4], oracle
        yield check, [255, 128, 254, 0], oracle
        yield check, [1, 1, 1, 1, 1, 1, 1, 1], oracle
