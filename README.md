# RawPDB

**RawPDB** is a C++11 library that directly reads Microsoft Program DataBase PDB files. The code is extracted almost directly from the upcoming 2.0 release of <a href="https://liveplusplus.tech/">Live++</a>.

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
* High-quality code - **RawPDB** compiles clean under -Wall

## Building

The code compiles clean under Visual Studio 2015, 2017, 2019, or 2022. A solution for Visual Studio 2019 is included.

## Performance

Running the **Symbols** and **Contributions** examples on a 1GiB PDB yields the following output:

<pre>
Opening PDB file C:\Development\llvm-project\build\tools\clang\unittests\Tooling\RelWithDebInfo\ToolingTests.pdb

Running example "Symbols"
| Reading image section stream
| ---> done in 0.066ms
| Reading module info stream
| ---> done in 0.562ms
| Reading symbol record stream
| ---> done in 25.185ms
| Reading public symbol stream
| ---> done in 1.133ms
| Storing public symbols
| ---> done in 46.171ms (212023 elements)
| Reading global symbol stream
| ---> done in 1.381ms
| Storing global symbols
| ---> done in 12.769ms (448957 elements)
| Storing symbols from modules
| ---> done in 145.849ms (2243 elements)
---> done in 233.694ms (539611 elements)
</pre>

<pre>
Opening PDB file C:\Development\llvm-project\build\tools\clang\unittests\Tooling\RelWithDebInfo\ToolingTests.pdb

Running example "Contributions"
| Reading image section stream
| ---> done in 0.066ms
| Reading module info stream
| ---> done in 0.594ms
| Reading section contribution stream
| ---> done in 9.839ms
| Storing contributions
| ---> done in 67.346ms (630924 elements)
| std::sort contributions
| ---> done in 19.218ms
---> done in 97.283ms
20 largest contributions:
1: 1896496 bytes from LLVMAMDGPUCodeGen.dir\RelWithDebInfo\AMDGPUInstructionSelector.obj
2: 1700720 bytes from LLVMHexagonCodeGen.dir\RelWithDebInfo\HexagonInstrInfo.obj
3: 1536470 bytes from LLVMRISCVCodeGen.dir\RelWithDebInfo\RISCVISelDAGToDAG.obj
4: 1441408 bytes from LLVMAArch64CodeGen.dir\RelWithDebInfo\AArch64InstructionSelector.obj
5: 1187048 bytes from LLVMRISCVCodeGen.dir\RelWithDebInfo\RISCVInstructionSelector.obj
6: 1026504 bytes from LLVMARMCodeGen.dir\RelWithDebInfo\ARMInstructionSelector.obj
7: 952080 bytes from LLVMAMDGPUDesc.dir\RelWithDebInfo\AMDGPUMCTargetDesc.obj
8: 849888 bytes from LLVMX86Desc.dir\RelWithDebInfo\X86MCTargetDesc.obj
9: 712176 bytes from LLVMHexagonCodeGen.dir\RelWithDebInfo\HexagonInstrInfo.obj
10: 679035 bytes from LLVMX86CodeGen.dir\RelWithDebInfo\X86ISelDAGToDAG.obj
11: 525174 bytes from LLVMAMDGPUDesc.dir\RelWithDebInfo\AMDGPUMCTargetDesc.obj
12: 523035 bytes from * Linker *
13: 519312 bytes from LLVMRISCVDesc.dir\RelWithDebInfo\RISCVMCTargetDesc.obj
14: 512496 bytes from LLVMVEDesc.dir\RelWithDebInfo\VEMCTargetDesc.obj
15: 498768 bytes from LLVMX86CodeGen.dir\RelWithDebInfo\X86InstructionSelector.obj
16: 483528 bytes from LLVMMipsCodeGen.dir\RelWithDebInfo\MipsInstructionSelector.obj
17: 449472 bytes from LLVMAMDGPUCodeGen.dir\RelWithDebInfo\AMDGPUISelDAGToDAG.obj
18: 444246 bytes from C:\Development\llvm-project\build\tools\clang\lib\Basic\obj.clangBasic.dir\RelWithDebInfo\DiagnosticIDs.obj
19: 371584 bytes from LLVMAArch64CodeGen.dir\RelWithDebInfo\AArch64ISelDAGToDAG.obj
20: 370272 bytes from LLVMNVPTXDesc.dir\RelWithDebInfo\NVPTXMCTargetDesc.obj
</pre>

Running the **Lines** example on a 1.37 GiB PDB yields the following output:

<pre>

Opening PDB file C:\pdb-test-files\clang-debug.pdb
Version 20000404, signature 1658696914, age 1, GUID 563dd8f1-f32b-459b-8c2beae0e70bc19b

Running example "Lines"
| Reading image section stream
| ---> done in 0.313ms
| Reading module info stream
| ---> done in 0.403ms
| Reading names stream
| ---> done in 0.126ms
| Storing lines from modules
| ---> done in 306.720ms (1847 elements)
| std::sort sections
| ---> done in 103.090ms (4023680 elements)

</pre>

This is at least an order of magnitude faster than DIA, even though the example code is completely serial and uses std::vector, std::string, and std::sort, which are used for illustration purposes only.

When reading streams in a concurrent fashion, you will most likely be limited by the speed at which the OS can bring the data into your process.

## Supported streams

**RawPDB** gives you access to the following PDB stream data:

* DBI stream data
	* Public symbols
	* Global symbols
	* Modules
	* Module symbols
	* Module lines (C13 line information)
	* Image sections
	* Info stream
		* /names stream
	* Section contributions
	* Source files

* IPI stream data

* TPI stream data

Furthermore, PDBs linked using /DEBUG:FASTLINK are not supported. These PDBs do not contain much information, since private symbol information is distributed among object files and library files.

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

### Symbols (<a href="https://github.com/MolecularMatters/raw_pdb/blob/main/src/Examples/ExampleSymbols.cpp">ExampleSymbols.cpp</a>)

A basic example that shows how to load and symbols from public, global, and module streams.

### Contributions (<a href="https://github.com/MolecularMatters/raw_pdb/blob/main/src/Examples/ExampleContributions.cpp">ExampleContributions.cpp</a>)

A basic example that shows how to load contributions, sort them by size, and output the 20 largest ones along with the object file they originated from.

### Function symbols (<a href="https://github.com/MolecularMatters/raw_pdb/blob/main/src/Examples/ExampleFunctionSymbols.cpp">ExampleFunctionSymbols.cpp</a>)

An example intended for profiler developers that shows how to enumerate all function symbols and retrieve or compute their code size.

## Sponsoring or supporting RawPDB

We have chosen a very liberal license to let **RawPDB** be used in as many scenarios as possible, including commercial applications. If you would like to support its development, consider licensing <a href="https://liveplusplus.tech/">Live++</a> instead. Not only do you give something back, but get a great productivity enhancement on top!
