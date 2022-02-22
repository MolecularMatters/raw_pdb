// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

extern unsigned char _BitScanForward(unsigned long* _Index, unsigned long _Mask);
#pragma intrinsic(_BitScanForward)


namespace BitUtil
{
	template <typename T>
	PDB_NO_DISCARD inline constexpr bool IsPowerOfTwo(T value) PDB_NO_EXCEPT
	{
		static_assert(std::is_unsigned<T>::value == true, "T must be an unsigned type.");

		PDB_ASSERT(value != 0u, "Invalid value.");

		return (value & (value - 1u)) == 0u;
	}


	template <typename T>
	PDB_NO_DISCARD inline constexpr T RoundUpToMultiple(T numToRound, T multipleOf) PDB_NO_EXCEPT
	{
		static_assert(std::is_unsigned<T>::value == true, "T must be an unsigned type.");

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

		unsigned long result = 0u;
		_BitScanForward(&result, value);

		return result;
	}
}

#define PDB_DEFINE_BIT_OPERATORS(_type)															\
	PDB_NO_DISCARD inline constexpr _type operator|(_type lhs, _type rhs) PDB_NO_EXCEPT			\
	{																							\
		return static_cast<_type>(PDB_AS_UNDERLYING(lhs) | PDB_AS_UNDERLYING(rhs));				\
	}																							\
																								\
	PDB_NO_DISCARD inline constexpr _type operator&(_type lhs, _type rhs) PDB_NO_EXCEPT			\
	{																							\
		return static_cast<_type>(PDB_AS_UNDERLYING(lhs) & PDB_AS_UNDERLYING(rhs));				\
	}																							\
																								\
	PDB_NO_DISCARD inline constexpr _type operator~(_type value) PDB_NO_EXCEPT					\
	{																							\
		return static_cast<_type>(~PDB_AS_UNDERLYING(value));									\
	}
