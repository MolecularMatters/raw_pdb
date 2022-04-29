// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "PDB_Platform.h"


#if PDB_COMPILER_MSVC
#	pragma warning(pop)
#elif PDB_COMPILER_CLANG
#	pragma clang diagnostic pop
#elif PDB_COMPILER_GCC
#	pragma GCC diagnostic pop
#endif