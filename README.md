# [Brutus](https://github.com/PLD-H4111-COMP/Brutus) [![Build Status](https://travis-ci.com/PLD-H4111-COMP/Brutus.svg?branch=master)](https://travis-ci.com/PLD-H4111-COMP/Brutus)

Brutus is a C compiler written in C++11 that supports :
- The following types : `void`, `char` (1 byte), `int` (8 bytes), `int16_t` (2 bytes), `int32_t` (4 bytes), `int64_t` (8 bytes).
- Conditonnal structures : `if`, `else`, `while`, `for`.
- The following operators with associativity and precedence : `=`, `+`, `-`, `*`, `/`, `%`, `||`, `&&`, `|`, `&`, `^`, `~`, `==`, `!=`, `<`, `<=`, `>`, `>=`, `!`, `++` (prefix only), `--` (prefix only).
- Order of evaluation of `||` and `&&`.
- Char litterals including `\a`, `\b`, `\f`, `\n`, `\r`, `\t`, `\v`, `\'`, `\"`, `\?`.
- Function definitions and calls with more than *6* paramaters.
- Abstract Syntax Tree (AST) and Intermediate Representation (IR).
- x86_64 asm generation.
- Warnings : uninitialized variables, unused variables/parameters, implicit declaration of function.

## How to build
You need GCC >= 5, cmake and git.

```
mkdir build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
```

## How to use

```
./Brutus [-o <output_file>] <input_file>
./Brutus --help
```

## How to compile

```
./compile.sh [-o <output_file>] <input_file>
```

## How to test

```
make test
./customTests.sh
./moodleTests.sh
```
