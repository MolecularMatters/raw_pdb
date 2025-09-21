// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "Foundation/PDB_Warnings.h"

// The following clang warnings must be disabled for the examples to build with 0 warnings
#if PDB_COMPILER_CLANG
#	pragma clang diagnostic ignored "-Wformat-nonliteral" 	// format string is not a string literal
#	pragma clang diagnostic ignored "-Wswitch-default" 		// switch' missing 'default' label
#   pragma clang diagnostic ignored "-Wcast-align"		 	// increases required alignment from X to Y
#   pragma clang diagnostic ignored "-Wold-style-cast" 		// use of old-style cast
#endif

#if PDB_COMPILER_MSVC
#	pragma warning(push, 0)
#elif PDB_COMPILER_CLANG
#	pragma clang diagnostic push
#endif

#if PDB_COMPILER_MSVC
	// we compile without exceptions
#	define _ALLOW_RTCc_IN_STL

	// triggered by Windows.h
#	pragma warning (disable : 4668)

	// triggered by xlocale in VS 2017
#	pragma warning (disable : 4625)		// copy constructor was implicitly defined as deleted
#	pragma warning (disable : 4626)		// assignment operator was implicitly defined as deleted
#	pragma warning (disable : 5026)		// move constructor was implicitly defined as deleted
#	pragma warning (disable : 5027)		// move assignment operator was implicitly defined as deleted
#	pragma warning (disable : 4774)		// format string expected in argument 1 is not a string literal
#endif

#ifdef _WIN32
#	define NOMINMAX
#	include <Windows.h>
#	undef cdecl
#endif
#	include <vector>
#	include <unordered_set>
#	include <chrono>
#	include <string>
#	include <algorithm>
#	include <cstdarg>

#if PDB_COMPILER_MSVC
#	pragma warning(pop)
#elif PDB_COMPILER_CLANG
#	pragma clang diagnostic pop
#endif
