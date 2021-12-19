# mulprec

> calculate pi with multiple precision arithmetic.

## Requirements

- C compiler
- CMake >= 3.1
- Python

## Usage

### Create build tree

```shell
cmake -B build
```

### Build

```shell
cmake --build build
```

### Start

```shell
python3 start.py
```

### Run tests

#### Run all test

```shell
./test.sh
```

#### Run specific test

```shell
# You don't need `test_` prefix
./test.sh <grep pattern>
```
