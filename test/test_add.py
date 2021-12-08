import random
import subprocess
import sys

TEST_COMMAND = "./build/test/test_add"

for _ in range(1000):
    x = random.randint(-1 * 10 ** 50, 10 ** 50)
    y = random.randint(-1 * 10 ** 50, 10 ** 50)
    res = subprocess.run([TEST_COMMAND, str(x), str(y)], stdout=subprocess.PIPE, stderr=sys.stderr).stdout
    expected = x + y
    actual = int(res)

    if expected != actual:
        print("Test failed", file=sys.stderr)
        print("  x       : {}".format(x), file=sys.stderr)
        print("  y       : {}".format(y), file=sys.stderr)
        print("  expected: {}".format(expected), file=sys.stderr)
        print("  actual  : {}".format(actual), file=sys.stderr)
        sys.exit(1)

print("Test passed")
