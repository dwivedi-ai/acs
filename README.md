```markdown:README.md
# ACS Language Compiler

Welcome to the ACS Language project. This document provides a brief overview of how to compile and use the ACS compiler to work with the ACS language, a custom language focusing on simplicity and efficiency. **Currently, the implementation supports only the `println` functionality, as this is just a testing compiler**, with plans to extend it further.

## Compilation

To compile the ACS compiler, you will need to have `g++` installed on your system. The ACS compiler can be compiled by executing the following command in the terminal:

```bash
g++ -I../include .cpp -o acs
```
This command compiles all `.cpp` files in the current directory, linking them with headers located in the `../include` directory, and produces an executable named `acs`.

## Usage

After compilation, you can use the `acs` executable to compile ACS language files. Since the current implementation primarily supports the print functionality, you can test it by creating a simple ACS file that uses the print command.

Example `hello.acs`:
```acs
println("hello, acs world");
```

To compile this file, simply run the `acs` executable with the ACS file as an argument. Note that further usage instructions will be provided as the language features are expanded.

## Future Implementations

The ACS language is in its early stages, with only the print functionality currently implemented. Future updates will include more language features such as variables, control structures, and more advanced data types. Stay tuned for updates.

## Contributing

Contributions to the ACS language are welcome. Whether you're interested in fixing bugs, adding new features, or improving documentation, your help is appreciated. Please feel free to fork the repository, make your changes, and submit a pull request.

Thank you for your interest in the ACS language project. We look forward to your contributions and to growing the capabilities of ACS together.