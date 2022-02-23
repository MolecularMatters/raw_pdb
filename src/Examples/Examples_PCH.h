// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

// we compile without exceptions
#define _ALLOW_RTCc_IN_STL
#include "Foundation/PDB_DisableWarningsPush.h"
#include <Windows.h>
#include <vector>
#include <chrono>
#include <string>
#include "Foundation/PDB_DisableWarningsPop.h"

// this needs to be the first include before including anything from the RawPDB library,
// so it makes sense to put it into the PCH for all examples
#include "PDB_ClientInclude.h"

// ignore purely informational warnings
#pragma warning (disable : 4514)		// unreferenced inline function has been removed
#pragma warning (disable : 4820)		// 'N' bytes padding added after data member 'm_member'
#pragma warning (disable : 5045)		// Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
