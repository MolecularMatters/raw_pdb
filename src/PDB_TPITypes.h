// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "Foundation/PDB_Macros.h"
#include "Foundation/PDB_BitOperators.h"
#include "Foundation/PDB_DisableWarningsPush.h"
#include <cstdint>
#include "Foundation/PDB_DisableWarningsPop.h"

namespace PDB
{
	namespace TPI
	{
		// https://llvm.org/docs/PDB/TpiStream.html#tpi-header
		struct StreamHeader
		{
			enum class PDB_NO_DISCARD Version : uint32_t
			{
				V40 = 19950410u,
				V41 = 19951122u,
				V50 = 19961031u,
				V70 = 19990903u,
				V80 = 20040203u
			};

			Version version;
			uint32_t headerSize;
			uint32_t typeIndexBegin;
			uint32_t typeIndexEnd;
			uint32_t typeRecordBytes;
			uint16_t hashStreamIndex;
			uint16_t hashAuxStreamIndex;
			uint32_t hashKeySize;
			uint32_t hashBucketCount;
			uint32_t hashValueBufferOffset;
			uint32_t hashValueBufferLength;
			uint32_t indexOffsetBufferOffset;
			uint32_t indexOffsetBufferLength;
			uint32_t hashAdjBufferOffset;
			uint32_t hashAdjBufferLength;
		};
	}
	namespace CodeView
	{
		namespace TPI
		{
			// code view type records that can appear in an TPI stream
			// https://llvm.org/docs/PDB/CodeViewTypes.html
			// https://llvm.org/docs/PDB/TpiStream.html#tpi-vs-ipi-stream
			enum class PDB_NO_DISCARD TypeRecordKind : uint16_t
			{
				LF_POINTER     = 0x1002u,
				LF_MODIFIER    = 0x1001u,
				LF_PROCEDURE   = 0x1008u,
				LF_MFUNCTION   = 0x1009u,
				LF_LABEL       = 0x000eu,
				LF_ARGLIST     = 0x1201u,
				LF_FIELDLIST   = 0x1203u,
				LF_ARRAY       = 0x1503u,
				LF_CLASS       = 0x1504u,
				LF_STRUCTURE   = 0x1505u,
				LF_INTERFACE   = 0x1519u,
				LF_UNION       = 0x1506u,
				LF_ENUM        = 0x1507u,
				LF_TYPESERVER2 = 0x1515u,
				LF_VFTABLE     = 0x151du,
				LF_VTSHAPE     = 0x000au,
				LF_BITFIELD    = 0x1205u,
				LF_METHODLIST  = 0x1206u,
				LF_PRECOMP     = 0x1509u,
				LF_ENDPRECOMP  = 0x0014u,
			};

			struct RecordHeader
			{
				uint16_t size;					// record length, not including this 2-byte field
				TypeRecordKind kind;			// record kind
			};

			struct Record
			{
				RecordHeader header;
				union Data
				{
#pragma pack(push, 1)
#pragma pack(pop)
				} data;
			};
		}
	}
}
