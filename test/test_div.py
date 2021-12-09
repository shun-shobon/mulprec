import random
import subprocess
import sys

TEST_COMMAND = "./build/test/test_div"

for _ in range(1000):
    x = random.randint(0, 10 ** 50)
    y = random.randint(1, 10 ** 50)
    res = subprocess.run([TEST_COMMAND, str(x), str(y)], stdout=subprocess.PIPE, stderr=sys.stderr).stdout
    expected_div, expected_mod = x // y, x % y
    actual_div, actual_mod = map(int, res.splitlines())

    if expected_div != actual_div or expected_mod != actual_mod:
        print("Test failed", file=sys.stderr)
        print("  x           : {}".format(x), file=sys.stderr)
        print("  y           : {}".format(y), file=sys.stderr)
        print("  expected div: {}".format(expected_div), file=sys.stderr)
        print("  actual div  : {}".format(actual_div), file=sys.stderr)
        print("  expected mod: {}".format(expected_mod), file=sys.stderr)
        print("  actual mod  : {}".format(actual_mod), file=sys.stderr)
        sys.exit(1)

print("Test passed")
