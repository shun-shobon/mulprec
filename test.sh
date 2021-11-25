#!/usr/bin/env sh

set -ue

error() {
	echo "$0: $@" >&2
}

cd "$(dirname "$0")"

test_dir="build/test"
pattern="test_${1:-*}"

if [ ! -d $test_dir ]; then
  error "$test_dir does not exist. please run cmake and build."
	exit 1
fi

find $test_dir -type f -maxdepth 1 -name "$pattern" | xargs -t -P 4 command
