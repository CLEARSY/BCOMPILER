# B Compiler

This git repository is dedicated to C++-based compiler tools for B:

- `bxml`: produces BXML files representing the decorated abstract syntax tree of B components.
  It is built upon the `bcomp` tool.
- `bcomp`: front-end parser and type checker for B source files.

## Build instructions

### Linux

```sh
git clone https://github.com/CLEARSY/bcompiler bcompiler
cd bcompiler
mkdir -p build
cd build
cmake ..
cmake --build .
```

## Licensing

This code is distributed under the license: "GNU GENERAL PUBLIC LICENSE, Version 3".
See the LICENSE file in this repository.
