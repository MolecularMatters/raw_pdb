// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once


// avoid pulling in different headers just for a few declarations
extern "C" int __cdecl printf(char const* const _Format, ...);

extern "C" int __cdecl memcmp(void const* _Buf1, void const* _Buf2, size_t  _Size);
extern "C" void* __cdecl memcpy(void* _Dst, void const* _Src, size_t  _Size);

extern "C" size_t __cdecl strlen(char const* _Str);
extern "C" int __cdecl strcmp(char const* _Str1, char const* _Str2);
