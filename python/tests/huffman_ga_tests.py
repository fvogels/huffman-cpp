import sys
sys.path.append('..')

from huffman_ga import GrowingTreeAdaptiveHuffmanEncoding


def test_adaptive_huffman_encoding_with_growing_tree():
    def check(data):
        encoding = GrowingTreeAdaptiveHuffmanEncoding(256)
        data = [ *data.encode('ascii') ]
        encoded = encoding.encode(data)
        decoded = list(encoding.decode(encoded))
        assert data == decoded, f'data={data}, decoded={decoded}'
    yield check, 'abc'
    yield check, 'aabbbc'
    yield check, 'aababcabcdabcde'
    yield check, 'abbccccdddddddd'
    yield check, 'acndjlkajcipocidjfdjslkfjsfjoijciojdiocjaiojcoisdjiojaiocjiojoijcio' * 10
