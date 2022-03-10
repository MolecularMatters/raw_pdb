// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Foundation/PDB_Macros.h"
#include <chrono>


class TimedScope
{
public:
	explicit TimedScope(const char* message);

	void Done(void) const;
	void Done(size_t count) const;

private:
	double ReadMilliseconds(void) const;

	const std::chrono::high_resolution_clock::time_point m_begin;

	PDB_DISABLE_COPY_MOVE(TimedScope);
};
