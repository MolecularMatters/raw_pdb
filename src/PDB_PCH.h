// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once


// ------------------------------------------------------------------------------------------------
// PLATFORM
// ------------------------------------------------------------------------------------------------

// determine the compiler used
#if defined(__clang__)
#	define PDB_COMPILER_MSVC				0
#	define PDB_COMPILER_CLANG				1
#elif defined(_MSC_VER)
#	define PDB_COMPILER_MSVC				1
#	define PDB_COMPILER_CLANG				0
#else
#	error("Unknown compiler.");
#endif


// ------------------------------------------------------------------------------------------------
// ATTRIBUTES
// ------------------------------------------------------------------------------------------------

// Indicates to the compiler that the function returns an object that is not aliased by any other pointers.
#define PDB_NO_ALIAS								__declspec(restrict)

// Indicates to the compiler that the return value of a function or class should not be ignored.
#define PDB_NO_DISCARD								[[nodiscard]]

// Indicates to the compiler that a function does not throw an exception.
#define PDB_NO_EXCEPT								noexcept


// ------------------------------------------------------------------------------------------------
// SPECIAL MEMBER FUNCTIONS
// ------------------------------------------------------------------------------------------------

// Default special member functions.
#define PDB_DEFAULT_COPY_CONSTRUCTOR(_name)			_name(const _name&) PDB_NO_EXCEPT = default
#define PDB_DEFAULT_COPY_ASSIGNMENT(_name)			_name& operator=(const _name&) PDB_NO_EXCEPT = default
#define PDB_DEFAULT_MOVE_CONSTRUCTOR(_name)			_name(_name&&) PDB_NO_EXCEPT = default
#define PDB_DEFAULT_MOVE_ASSIGNMENT(_name)			_name& operator=(_name&&) PDB_NO_EXCEPT = default

// Default copy member functions.
#define PDB_DEFAULT_COPY(_name)						PDB_DEFAULT_COPY_CONSTRUCTOR(_name); PDB_DEFAULT_COPY_ASSIGNMENT(_name)

// Default move member functions.
#define PDB_DEFAULT_MOVE(_name)						PDB_DEFAULT_MOVE_CONSTRUCTOR(_name); PDB_DEFAULT_MOVE_ASSIGNMENT(_name)

// Single macro to default all copy and move member functions.
#define PDB_DEFAULT_COPY_MOVE(_name)				PDB_DEFAULT_COPY(_name); PDB_DEFAULT_MOVE(_name)

// Disable special member functions.
#define PDB_DISABLE_COPY_CONSTRUCTOR(_name)			_name(const _name&) PDB_NO_EXCEPT = delete
#define PDB_DISABLE_COPY_ASSIGNMENT(_name)			_name& operator=(const _name&) PDB_NO_EXCEPT = delete
#define PDB_DISABLE_MOVE_CONSTRUCTOR(_name)			_name(_name&&) PDB_NO_EXCEPT = delete
#define PDB_DISABLE_MOVE_ASSIGNMENT(_name)			_name& operator=(_name&&) PDB_NO_EXCEPT = delete

// Disable copy member functions.
#define PDB_DISABLE_COPY(_name)						PDB_DISABLE_COPY_CONSTRUCTOR(_name); PDB_DISABLE_COPY_ASSIGNMENT(_name)

// Disable move member functions.
#define PDB_DISABLE_MOVE(_name)						PDB_DISABLE_MOVE_CONSTRUCTOR(_name); PDB_DISABLE_MOVE_ASSIGNMENT(_name)

// Single macro to disable all copy and move member functions.
#define PDB_DISABLE_COPY_MOVE(_name)				PDB_DISABLE_COPY(_name); PDB_DISABLE_MOVE(_name)


// ------------------------------------------------------------------------------------------------
// COMPILER WARNINGS
// ------------------------------------------------------------------------------------------------

#if PDB_COMPILER_MSVC
#	define PDB_PRAGMA(_x)									__pragma(_x)

#	define PDB_PUSH_WARNING_MSVC							PDB_PRAGMA(warning(push))
#	define PDB_SUPPRESS_WARNING_MSVC(_number)				PDB_PRAGMA(warning(suppress : _number))
#	define PDB_DISABLE_WARNING_MSVC(_number)				PDB_PRAGMA(warning(disable : _number))
#	define PDB_POP_WARNING_MSVC								PDB_PRAGMA(warning(pop))

#	define PDB_PUSH_WARNING_CLANG
#	define PDB_DISABLE_WARNING_CLANG(_diagnostic)
#	define PDB_POP_WARNING_CLANG
#elif PDB_COMPILER_CLANG
#	define PDB_PRAGMA(_x)									_Pragma(#_x)

#	define PDB_PUSH_WARNING_MSVC
#	define PDB_SUPPRESS_WARNING_MSVC(_number)
#	define PDB_DISABLE_WARNING_MSVC(_number)
#	define PDB_POP_WARNING_MSVC

#	define PDB_PUSH_WARNING_CLANG							PDB_PRAGMA(clang diagnostic push)
#	define PDB_DISABLE_WARNING_CLANG(_diagnostic)			PDB_PRAGMA(clang diagnostic ignored _diagnostic)
#	define PDB_POP_WARNING_CLANG							PDB_PRAGMA(clang diagnostic pop)
#endif


// ------------------------------------------------------------------------------------------------
// MISCELLANEOUS
// ------------------------------------------------------------------------------------------------

// Defines a C-like flexible array member.
#define PDB_FLEXIBLE_ARRAY_MEMBER(_type, _name)					\
	PDB_PUSH_WARNING_MSVC										\
	PDB_PUSH_WARNING_CLANG										\
	PDB_DISABLE_WARNING_MSVC(4200)								\
	PDB_DISABLE_WARNING_CLANG("-Wc99-extensions")				\
	PDB_DISABLE_WARNING_CLANG("-Wmicrosoft-flexible-array")		\
	_type _name[];												\
	PDB_POP_WARNING_MSVC										\
	PDB_POP_WARNING_CLANG

// Casts any value to the value of the underlying type.
#define PDB_AS_UNDERLYING(_value)								static_cast<typename std::underlying_type<decltype(_value)>::type>(_value)


// ------------------------------------------------------------------------------------------------
// WARNING SETTINGS
// ------------------------------------------------------------------------------------------------

#if PDB_COMPILER_MSVC
	// we compile with exceptions disabled
#	pragma warning(disable : 4577)		// warning C4577: 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed. Specify /EHsc

	// ignore purely informational warnings
#	pragma warning (disable : 4514)		// unreferenced inline function has been removed
#	pragma warning (disable : 4710)		// function not inlined
#	pragma warning (disable : 4711)		// function selected for automatic inline expansion
#	pragma warning (disable : 4820)		// 'N' bytes padding added after data member 'm_member'
#	pragma warning (disable : 5045)		// Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
#elif PDB_COMPILER_CLANG
	// turn on absolutely all available Clang warnings
#	pragma clang diagnostic warning "-Wall"
#	pragma clang diagnostic warning "-Wextra"
#	pragma clang diagnostic warning "-Weverything"
#	pragma clang diagnostic warning "-Wpedantic"

	// these warnings contradict -Weverything
#	pragma clang diagnostic ignored "-Wc++98-compat"
#	pragma clang diagnostic ignored "-Wc++98-compat-pedantic"

	// this warning is triggered for templates which are explicitly instantiated.
	// forgetting to instantiate the template would trigger a linker error anyway, so we disable this warning.
#	pragma clang diagnostic ignored "-Wundefined-func-template"

	// we don't strive for C++20 compatibility
#	pragma clang diagnostic ignored "-Wc++20-compat"

	// some structures will have to be padded
#	pragma clang diagnostic ignored "-Wpadded"
#endif


// ------------------------------------------------------------------------------------------------
// INCLUDES
// ------------------------------------------------------------------------------------------------

// third-party includes
#include "PDB_DisableWarningsPush.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <type_traits>
#include "PDB_DisableWarningsPop.h"

// library includes
#include "Foundation/PDB_Log.h"
#include "Foundation/PDB_Assert.h"
#include "Foundation/PDB_Move.h"
#include "Foundation/PDB_Forward.h"
#include "Foundation/PDB_Memory.h"
#include "Foundation/PDB_ArrayView.h"
#include "Foundation/PDB_BitUtil.h"
#include "Foundation/PDB_PointerUtil.h"
