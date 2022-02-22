// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once


// https://llvm.org/docs/PDB/index.html
namespace PDB
{
	class RawFile;


	// Validates whether a PDB file is valid.
	PDB_NO_DISCARD bool ValidateFile(const wchar_t* const path, const void* data) PDB_NO_EXCEPT;

	// Creates a raw PDB file that must have been validated.
	PDB_NO_DISCARD RawFile CreateRawFile(const void* data) PDB_NO_EXCEPT;
}
