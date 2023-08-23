# KreCalc
A calculator written in C++ capable of calculating complex expressions

It may be a bit messy because I decided to challenge myself and not really read up on the topic.
The only thing I knew before making it is that I would need to create a Abstract Syntax Tree

## Features
Currently available operations:
- '+' Addition
- '-' Subtraction
- '*' Multiplication
- '/' Division
- '^' Exponantiation
- '%' Modulus (Only works on integers and negative values are calculated like c++ would by default)

All operations are calculated using 64-bit floats (long double)

## Usage
```sh
$ ./krecalc "<expression>"
```
For example:
```sh
$ ./krecalc "3*(4+2)+7^4-8/3"
$ 2416.33
```

## Planned Features
- Functions like logarithms and the binomial coefficient
- Calculating the factorial
- Support for interpretation of scientific notation ex. 3e4 or 4.5e+12
- Fixing messy code like the current workaround for negative numbers
- Fixing bugs :>
- Ability to display the answer in full notation rather than scientific
