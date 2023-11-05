// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "Foundation/PDB_Warnings.h"
#include "Foundation/PDB_DisableWarningsPush.h"

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
#	include "Foundation/PDB_DisableWarningsPop.h"
