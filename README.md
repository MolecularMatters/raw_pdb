# RawPDB

**RawPDB** is a C++17 library that directly reads Microsoft Program Debug Database PDB files. The code is extracted almost directly from the upcoming 2.0 release of <a href="https://liveplusplus.tech/">Live++</a>.

## Design

**RawPDB** gives you direct access to the stream data contained in a PDB file. It does not attempt to offer abstractions for iterating symbols, translation units, contributions, etc.

Building a high-level abstraction over the provided low-level data is an ill-fated attempt that can never really be performant for everybody, because different tools like debuggers, hot-reload tools (e.g. <a href="https://liveplusplus.tech/">Live++</a>), profilers (e.g. <a href="https://superluminal.eu/">Superluminal</a>), need to perform different queries against the stored data.

We therefore believe the best solution is to offer direct access to the underlying data, with applications bringing that data into their own structures.

## Goal

Eventually, we want **RawPDB** to become the de-facto replacement of <a href="https://docs.microsoft.com/en-us/visualstudio/debugger/debug-interface-access/debug-interface-access-sdk">Microsoft's DIA SDK</a> that most C++ developers (have to) use.

## Features

* Fast - **RawPDB** works directly with memory-mapped data, so only the data from the streams you touch affect performance. It is orders of magnitudes faster than the DIA SDK, and faster than comparable LLVM code
* Scalable - **RawPDB's** API gives you access to individual streams that can all be read concurrently in a trivial fashion, since all returned data structures are immutable. There are no locks or waits anywhere inside the library
* Lightweight - **RawPDB** is small and compiles in roughly 1 second
* Allocation-friendly - **RawPDB** performs only a few allocations, and those can be overridden easily by changing the underlying macro
* No STL - **RawPDB** does not need any STL containers or algorithms
* No exceptions - **RawPDB** does not use exceptions
* No RTTI - **RawPDB** does not need RTTI or use class hierarchies
* High quality code - **RawPDB** compiles clean under -Wall

## Performance

Running the **Symbols** example on a 2GiB PDB yields the following output:

<pre>
Opening PDB file C:\dev\2GiB.pdb
Reading image section stream...done
Reading module info stream...done
Reading section contribution stream...done
Storing 3780654 section contributions...done
Reading symbol record stream...done
Reading public symbol stream...done
Parsing 1804791 public symbols...done
Reading global symbol stream...done
Parsing 3948114 global symbols...done
Reading and parsing 4848 module streams...done
Stored 3798885 symbols in std::vector using std::string
Running time: 1679.323ms
</pre>

This is at least an order of magnitude faster than DIA, even though the example code is completely serial and uses std::vector and std::string, which are used for illustration purposes only.

When reading streams in a concurrent fashion, you will most likely be limited by the speed at which the OS can bring the data into your process.

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

## Sponsoring or supporting RawPDB

We have chosen a very liberal license to let **RawPDB** be used in as many scenarios as possible, including commercial applications. If you would like to support its development, consider licensing <a href="https://liveplusplus.tech/">Live++</a> instead. Not only do you give something back, but get a great productivity enhancement on top!
