
#Quine-McCluskey implementation in C++

This program implements the Quine-McCluskey algorithm for multiple 
output functions. The program outputs ASCII or LaTeX tables.
So far, it does not solve the minimum coverage problem yet.

This program is primarily being written for educational purpose for
visualising the algoirithm and copying the output to reports.

## Compile

To compile, open a terminal and type:

    g++ qm.cpp -o qm

## Usage

The program uses a specific input format:

    [number of bits]
    [number of output functions]
    [number of care values]
    [values seperated by space]
    [number of don't-care values]
    [don't-care values seperated by space]
    ... // repeat for as many output functions as needed

Example input:

    4
    3
    2
    2 3
    6
    10 11 12 13 14 15
    5
    1 2 3 5 8
    6
    10 11 12 13 14 15
    3
    6 7 8
    6
    10 11 12 13 14 15

## License

This code is distributed under the zlib license.

## Bugs

If you encounter any bugs, please report them on this GitHub project.
Or fix them yourself and make a Pull Request.
