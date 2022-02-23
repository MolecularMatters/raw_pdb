// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleTimedScope.h"


TimedScope::TimedScope(const char* scope)
	: m_scope(scope)
	, m_begin(std::chrono::high_resolution_clock::now())
{
}


void TimedScope::Print(void) const
{
	const std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<float> seconds = now - m_begin;
	const double milliSeconds = seconds.count() * 1000.0;

	printf("[PERF] Scope \"%s\" took %.3fms\n", m_scope, milliSeconds);
}
