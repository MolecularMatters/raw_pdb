// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once


// provide our own type traits to avoid pulling in unnecessary includes
namespace PDB
{
	template <class T>
	struct is_pointer
	{
		static constexpr bool value = false;
	};

	template <class T>
	struct is_pointer<T*>
	{
		static constexpr bool value = true;
	};

	template <class T>
	struct is_pointer<T* const>
	{
		static constexpr bool value = true;
	};

	template <class T>
	struct is_pointer<T* volatile>
	{
		static constexpr bool value = true;
	};

	template <class T>
	struct is_pointer<T* const volatile>
	{
		static constexpr bool value = true;
	};


	template <class T>
	struct remove_reference
	{
		using type = T;
	};

	template <class T>
	struct remove_reference<T&>
	{
		using type = T;
	};

	template <class T>
	struct remove_reference<T&&>
	{
		using type = T;
	};


	template <class T>
	struct underlying_type
	{
		using type = __underlying_type(T);
	};
}
