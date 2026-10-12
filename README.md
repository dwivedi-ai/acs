# ACS Language Compiler

Welcome to the ACS Language project. This document provides a brief overview of how to compile and use the ACS compiler to work with the ACS language, a custom language focusing on simplicity and efficiency. **Currently, the implementation supports only the `println` functionality, as this is just a testing compiler**, with plans to extend it further.

## Compilation

To compile the ACS compiler, you will need to have `g++` installed on your system. The ACS compiler can be compiled by executing the following command in the terminal:

```bash
make
```
This builds the compiler from `src/` and `include/` and produces an executable named `acs` in the repository root.

## Tests

Each example in `tests/examples/` is an `.acs` file with a matching `.expected` file holding its exact output (and an optional `.input` file piped to stdin for `scan`). Run them all with:

```bash
make test
```

After an intentional change in output, regenerate the expected files with `make update-tests` and review the diff before committing. The same tests run in GitHub Actions on every push and pull request.

## Usage

After compilation, you can use the `acs` executable to compile ACS language files. Since the current implementation primarily supports the print functionality, you can test it by creating a simple ACS file that uses the print command or variable declaration (strings not supported right now) and usage in print as shown below.

### Syntax Examples

#### Variable Declaration:
```acs
let x: 10;
let y: 9.99;
int a: 100;
float b: 29.8;
double p: 29.629;
```

#### Printing Variables:
```acs
println(x);
println("Value of X: " << x);
print("Value of Y: " << y << "\n");
```

#### Printing Statements:
```acs
println("hello, world");
print("hello, world");
```

#### Input Statements:
```acs
scan(x);
scan(p);
```

#### Evaluating Expressions:
```acs
x: x + 10;
y: y / 3;
```

#### Evaluating Expressions inside Print Statements:
```acs
println("Value of X: " << x / 2);
print("Value of Y: " << y + 3 << "\n");
```

#### Conditionals (if / else):
```acs
int score: 72;

if (score >= 90) {
    println("Grade: A");
} else if (score >= 70) {
    println("Grade: B");
} else {
    println("Grade: C");
}
```

- The condition goes in parentheses and the body in braces. `{` must end the `if (...)` / `else` line, and `}` goes on its own line or before `else`.
- Comparison operators: `==`, `!=`, `<`, `<=`, `>`, `>=`. Combine conditions with `&&`, `||` and `!`, group them with parentheses, and use arithmetic (`+ - * /`) inside them.
- Use `==` to compare; a single `=` is an error (assignment in ACS is `:`).
- Any number of `else if` branches is allowed, `else` is optional, and blocks can be nested.
- Variables declared inside a block exist only in that block.

A complete example is in [`examples/if_else.acs`](examples/if_else.acs). After `make`, run it from the `examples` directory with `mkdir -p ../build && ../acs if_else.acs`. The test case `tests/examples/if_else.acs` covers nesting, `else` on its own line and block-scoped variables.

To compile this file, simply run the `acs` executable with the ACS file as an argument. Note that further usage instructions will be provided as the language features are expanded.

## Future Implementations

The ACS language is in its early stages, with variable declaration, printing strings, printing variables using `<<` like in `cout`, input, expressions and `if`/`else` conditionals currently implemented. Future updates will include more language features such as loops and more advanced data types. Stay tuned for updates.


## Contributing

Contributions to the ACS language are welcome. Whether you're interested in fixing bugs, adding new features, or improving documentation, your help is appreciated. Please feel free to fork the repository, make your changes, and submit a pull request.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.

Thank you for your interest in the ACS language project. We look forward to your contributions and to growing the capabilities of ACS together.