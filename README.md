# Synthium

This is the official repository for the Synthium systems programming language.

## What is Synthium?

Synthium is a systems programming language, that aims to offer a familiar mix between C, Rust, TypeScript and Go. It provides low-level features, while remaining fairly high-level.

The current repository is a port of the source code from the self-hosted compiler and future developments to the compiler (the self-hosted compiler has been deprecated in favor of this C port).

Here is a simple example of what you can expect in terms of syntax:

```rs
import "io";

fn main(argc: i32, argv: []string): i32 {
    io.printf("Hello, world!\n");

    return 0;
}
```

# Building the project

Clone the repository. CD into the directory where you cloned the repository to and run `make`. The result should be an executable named `synthiumc`.

# Roadmap

  * Lexer
  * Parser
  * Typechecking and resolving
  * IR
  * Codegen

# Note

The Synthium compiler is very barebones and should not by any means be used for production.