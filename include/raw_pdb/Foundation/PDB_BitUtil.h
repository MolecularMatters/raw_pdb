// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "PDB_Platform.h"
#include "PDB_Assert.h"

#include <type_traits>
#include <cstdint>
#if PDB_COMPILER_MSVC
#include <intrin.h>
#endif

namespace PDB
{
    namespace BitUtil
    {
        template <typename T>
        PDB_NO_DISCARD inline constexpr bool IsPowerOfTwo(T value) PDB_NO_EXCEPT
        {
            static_assert(std::is_unsigned<T>::value == true, "T must be an unsigned type.");

            PDB_ASSERT(value != 0u);

            return (value & (value - 1u)) == 0u;
        }


        template <typename T>
        PDB_NO_DISCARD inline constexpr T RoundUpToMultiple(T numToRound, T multipleOf) PDB_NO_EXCEPT
        {
            static_assert(std::is_unsigned<T>::value == true, "T must be an unsigned type.");

            PDB_ASSERT(IsPowerOfTwo(multipleOf));

            return (numToRound + (multipleOf - 1u)) & ~(multipleOf - 1u);
        }


        // Finds the position of the first set bit in the given value starting from the LSB, e.g. FindFirstSetBit(0b00000010) == 1.
        // This operation is also known as CTZ (Count Trailing Zeros).
        template <typename T>
        PDB_NO_DISCARD inline uint32_t FindFirstSetBit(T value) PDB_NO_EXCEPT;

        template <>
        PDB_NO_DISCARD inline uint32_t FindFirstSetBit(uint32_t value) PDB_NO_EXCEPT
        {
            PDB_ASSERT(value != 0u);

#if PDB_COMPILER_MSVC
            unsigned long result = 0u;
            _BitScanForward(&result, value);
            return result;
#elif PDB_COMPILER_CLANG || PDB_COMPILER_GCC
            return static_cast<uint32_t>(__builtin_ctz(value));
#else
            static_assert(false, "Unsupported compiler");
#endif
        }
    }
}
