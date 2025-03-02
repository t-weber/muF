# $\mathrm{\mu F}$
A compiler and virtual machine for a small F90-inspired language.  
[![DOI: 10.5281/zenodo.14955118](https://zenodo.org/badge/DOI/10.5281/zenodo.14955118.svg)](https://doi.org/10.5281/zenodo.14955118)

## Dependencies
 - Install the LALR(1) library: https://github.com/t-weber/lalr1.
 - Install Mathlibs: https://github.com/t-weber/mathlibs.

## Building
 - Create the build directory: `mkdir build && pushd build`.
 - Build the parser generator: `cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(($(nproc)/2+1))`.
 - Generate the parser by running `./parsergen` from the build directory.
 - Re-run `cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(($(nproc)/2+1))` to build the actual compiler.

## Test
 - Compile an example program using `./compile ../test/comb.muf`.
 - Run the program using `./vm comb.bin`.
