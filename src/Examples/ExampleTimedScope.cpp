// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleTimedScope.h"

namespace
{
	static unsigned int g_indent = 0u;

	static void PrintIndent(void)
	{
		printf("%.*s", g_indent * 2u, "| | | | | | | | ");
	}
}


TimedScope::TimedScope(const char* message)
	: m_begin(std::chrono::high_resolution_clock::now())
{
	PrintIndent();
	++g_indent;

	printf("%s\n", message);
}


void TimedScope::Done(void) const
{
	--g_indent;
	PrintIndent();

	const double milliSeconds = ReadMilliseconds();
	printf("---> done in %.3fms\n", milliSeconds);
}


void TimedScope::Done(size_t count) const
{
	--g_indent;
	PrintIndent();

	const double milliSeconds = ReadMilliseconds();
	printf("---> done in %.3fms (%zu elements)\n", milliSeconds, count);
}


double TimedScope::ReadMilliseconds(void) const
{
	const std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double> seconds = now - m_begin;
	
	return seconds.count() * 1000.0;
}
