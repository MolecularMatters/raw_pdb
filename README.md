# RawPDB

**RawPDB** is a C++17 library that directly reads Microsoft Program Debug Database PDB files. The code is extracted almost directly from the upcoming 2.0 release of <a href="https://liveplusplus.tech/">Live++</a>.

## Design

**RawPDB** gives you direct access to the stream data contained in a PDB file. It does not attempt to offer abstractions for iterating symbols, translation units, contributions, etc.

Building a high-level abstraction over the provided low-
level data is an ill-fated attempt that can never really be performant for everybody, because different tools like debuggers, hot-reload tools (e.g. <a href="https://liveplusplus.tech/">Live++</a>), profilers (e.g. <a href="https://superluminal.eu/">Superluminal</a>), need to perform different queries against the stored data.

We therefore believe the best solution is to offer direct access to the underlying data, with applications bringing that data into their own structures.

## Goal

Eventually, we want **RawPDB** to become the de-facto replacement of <a href="https://docs.microsoft.com/en-us/visualstudio/debugger/debug-interface-access/debug-interface-access-sdk">Microsoft's DIA SDK</a> that most C++ developers (have to) use.

## Features

* Fast - **RawPDB** works directly with memory-mapped data, so only the data from the streams you touch affect performance. It is orders of magnitudes faster than the DIA SDK, and faster than comparable LLVM code
* Lightweight - **RawPDB** is small and compiles in roughly 1 second
* Allocation-friendly - **RawPDB** performs only a few allocations, and those can be overridden easily by changing the underlying macro
* No STL - **RawPDB** does not need any STL containers or algorithms
* No exceptions - **RawPDB** does not use exceptions
* No RTTI - **RawPDB** does not need RTTI or use class hierarchies
* High quality code - **RawPDB** compiles clean under -Wall

## Supported streams

**RawPDB** gives you access to the following PDB stream data:

* DBI stream data
	* Public symbols
	* Global symbols
	* Modules
	* Module symbols
	* Image sections
	* Info stream
	* Section contributions
	* Source files

* IPI stream data

At the moment, there is no support for C13 line information and the TPI type stream data, because Live++ does not make use of that information yet. However, we will gladly accept PRs, or implement support in the future.

## Documentation

If you are unfamiliar with the basic structure of a PDB file, the <a href="https://llvm.org/docs/PDB/index.html">LLVM documentation</a> serves as a good introduction.

Consult the example code to see how to read and parse the PDB streams.

## Directory structure

* bin: contains final binary output files (.exe and .pdb)
* build: contains Visual Studio 2019 solution and project files
* lib: contains the RawPDB library output files (.lib and .pdb)
* src: contains the RawPDB source code, as well as example code
* temp: contains intermediate build artefacts

## Examples

### Symbols

A simple example that shows how to load contributions and symbols from public, global, and module streams.
