// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "PDB_TypeTraits.h"


// See Jonathan Mueller's blog for replacing std::move and std::forward:
// https://foonathan.net/2020/09/move-forward/
#define PDB_MOVE(...)		static_cast<PDB::remove_reference<decltype(__VA_ARGS__)>::type&&>(__VA_ARGS__)
