// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once


// determine the compiler/toolchain used
#if defined(__clang__)
#	define PDB_COMPILER_MSVC				0
#	define PDB_COMPILER_CLANG				1
#elif defined(_MSC_VER)
#	define PDB_COMPILER_MSVC				1
#	define PDB_COMPILER_CLANG				0
#else
#	error("Unknown compiler.");
#endif
