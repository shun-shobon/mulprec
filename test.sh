#!/usr/bin/env sh

set -ue

error() {
	echo "$0: $*" >&2
}

cd "$(dirname "$0")"

test_dir="test"
pattern="test_${1:-*}.py"

if [ ! -d $test_dir ]; then
  error "$test_dir does not exist. please run cmake and build."
	exit 1
fi

# shellcheck disable=SC2044
for test in $(find $test_dir -type f -maxdepth 1 -name "$pattern"); do
  echo "Testing $test"
  python3 "$test"
  echo ""
done;