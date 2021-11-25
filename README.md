# mulprec

> calculate pi with multiple precision arithmetic.

## Requirements

- C compiler
- CMake >= 3.1

## Usage

### Create build tree

```shell
cmake -B build
```

### Build

```shell
cmake --build build
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
