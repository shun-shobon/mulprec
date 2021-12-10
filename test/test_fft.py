import random
import subprocess
import sys
from time import time

import numpy as np

TEST_COMMAND = "./build/test/test_fft"

time_sum = 0

loop = 1000

for i in range(loop):
    n = random.randint(1, 3000)
    f = [random.randint(0, 2 ** 10) for _ in range(n)]
    g = [random.randint(0, 2 ** 10) for _ in range(n)]
    start = time()
    res = subprocess.run(TEST_COMMAND, input=f"{n} {n} {' '.join(map(str, f))} {' '.join(map(str, g))}".encode("utf-8"),
                         stdout=subprocess.PIPE,
                         stderr=sys.stderr).stdout
    end = time()
    time_sum += end - start
    expected = np.convolve(f, g).tolist()
    actual = list(map(int, res.decode("utf-8").split()))

    if expected != actual:
        print("Test failed", file=sys.stderr)
        print("  x       : {}".format(f), file=sys.stderr)
        print("  y       : {}".format(g), file=sys.stderr)
        print("  expected: {}".format(expected), file=sys.stderr)
        print("  actual  : {}".format(actual), file=sys.stderr)
        sys.exit(1)

print("time average: {}ms".format((time_sum / loop) * 1000))

print("Test passed")
