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

Other Features:
- All operations are calculated using 64-bit floats (c++ long double)
- Support for scientific notation (eg. 1e5, 1e+5, 1e-5)
- No need to put * inbetween a number and brackets

## Usage
```sh
$ ./krecalc "<expression>"
```
For example:
```sh
$ ./krecalc "3(4+2) + 3^2 * 3.2e-3"
$ 18.0288
```

## Planned Features
- Support for flags
- Functions like logarithms and the binomial coefficient
- Calculating the factorial
- Fixing messy code like the current workaround for negative numbers
- Fixing bugs :>
- Ability to display the answer in full notation rather than scientific
- Error Handling
