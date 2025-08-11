// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "PDB_Assert.h"

#ifdef _WIN32
	PDB_PUSH_WARNING_CLANG
	PDB_DISABLE_WARNING_CLANG("-Wreserved-identifier")

	extern "C" unsigned char _BitScanForward(unsigned long* _Index, unsigned long _Mask);

	PDB_POP_WARNING_CLANG

#	if PDB_COMPILER_MSVC
#		pragma intrinsic(_BitScanForward)
#	endif
#endif


namespace PDB
{
	namespace BitUtil
	{
		// Returns whether the given unsigned value is a power of two.
		template <typename T>
		PDB_NO_DISCARD inline constexpr bool IsPowerOfTwo(T value) PDB_NO_EXCEPT
		{
			PDB_ASSERT(value != 0u, "Invalid value.");

			return (value & (value - 1u)) == 0u;
		}


		// Rounds the given unsigned value up to the next multiple.
		template <typename T>
		PDB_NO_DISCARD inline constexpr T RoundUpToMultiple(T numToRound, T multipleOf) PDB_NO_EXCEPT
		{
			PDB_ASSERT(IsPowerOfTwo(multipleOf), "Multiple must be a power-of-two.");

			return (numToRound + (multipleOf - 1u)) & ~(multipleOf - 1u);
		}


		// Finds the position of the first set bit in the given value starting from the LSB, e.g. FindFirstSetBit(0b00000010) == 1.
		// This operation is also known as CTZ (Count Trailing Zeros).
		template <typename T>
		PDB_NO_DISCARD inline uint32_t FindFirstSetBit(T value) PDB_NO_EXCEPT;

		template <>
		PDB_NO_DISCARD inline uint32_t FindFirstSetBit(uint32_t value) PDB_NO_EXCEPT
		{
			PDB_ASSERT(value != 0u, "Invalid value.");

#ifdef _WIN32
			unsigned long result = 0ul;

			_BitScanForward(&result, value);
#else
			unsigned int result = 0u;

			result = static_cast<unsigned int>(__builtin_ffs(static_cast<int>(value)));
			if (result)
			{
				--result;
			}
#endif

			return result;
		}
	}
}
