// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "PDB_Macros.h"
#include "PDB_Log.h"


PDB_PUSH_WARNING_CLANG
PDB_DISABLE_WARNING_CLANG("-Wgnu-zero-variadic-macro-arguments")
PDB_DISABLE_WARNING_CLANG("-Wreserved-identifier")

extern "C" void __cdecl __debugbreak(void);

#if PDB_COMPILER_MSVC
#	pragma intrinsic(__debugbreak)
#endif


#ifdef _DEBUG
#	define PDB_ASSERT(_condition, _msg, ...)			(_condition) ? (void)true : (PDB_LOG_ERROR(_msg, ##__VA_ARGS__), __debugbreak())
#else
#	define PDB_ASSERT(_condition, _msg, ...)			PDB_NOOP(_condition, _msg, ##__VA_ARGS__)
#endif

PDB_POP_WARNING_CLANG
