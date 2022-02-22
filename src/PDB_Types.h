// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once


namespace PDB
{
	struct GUID
	{
		unsigned long  Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned char  Data4[8];
	};

	// https://llvm.org/docs/PDB/MsfFile.html#msf-superblock
	struct PDB_NO_DISCARD SuperBlock
	{
		// https://github.com/Microsoft/microsoft-pdb/blob/master/PDB/msf/msf.cpp#L962
		static inline constexpr const char MAGIC[30u] = "Microsoft C/C++ MSF 7.00\r\n\x1a\x44\x53";

		char fileMagic[30u];
		char padding[2u];
		uint32_t blockSize;
		uint32_t freeBlockMapIndex;				// index of the free block map
		uint32_t blockCount;					// number of blocks in the file
		uint32_t directorySize;					// size of the stream directory in bytes
		uint32_t unknown;
		uint32_t directoryIndicesBlockIndex;	// index of the block that contains an array of indices of directory blocks
	};

	// https://llvm.org/docs/PDB/PdbStream.html#stream-header
	struct Header
	{
		enum class PDB_NO_DISCARD Version : uint32_t
		{
			VC2 = 19941610u,
			VC4 = 19950623u,
			VC41 = 19950814u,
			VC50 = 19960307u,
			VC98 = 19970604u,
			VC70Dep = 19990604u,
			VC70 = 20000404u,
			VC80 = 20030901u,
			VC110 = 20091201u,
			VC140 = 20140508u
		};

		Version version;
		uint32_t signature;
		uint32_t age;
		GUID guid;
	};

	// https://llvm.org/docs/PDB/PdbStream.html
	struct NamedStreamMap
	{
		uint32_t length;
		PDB_FLEXIBLE_ARRAY_MEMBER(char, stringTable);

		struct HashTableEntry
		{
			uint32_t stringTableOffset;
			uint32_t streamIndex;
		};
	};

	// https://llvm.org/docs/PDB/HashTable.html
	struct SerializedHashTable
	{
		struct Header
		{
			uint32_t size;
			uint32_t capacity;
		};

		struct BitVector
		{
			uint32_t wordCount;
			PDB_FLEXIBLE_ARRAY_MEMBER(uint32_t, words);
		};
	};

	// https://llvm.org/docs/PDB/PdbStream.html#pdb-feature-codes
	enum class PDB_NO_DISCARD FeatureCode : uint32_t
	{
		VC110 = 20091201,
		VC140 = 20140508,

		// https://github.com/microsoft/microsoft-pdb/blob/master/PDB/include/pdbcommon.h#L23
		NoTypeMerge = 0x4D544F4E,				// "NOTM"
		MinimalDebugInfo = 0x494E494D			// "MINI", i.e. executable was linked with /DEBUG:FASTLINK
	};

	// header of the public stream, based on PSGSIHDR defined here:
	// https://github.com/Microsoft/microsoft-pdb/blob/master/PDB/dbi/gsi.h#L240
	struct PublicStreamHeader
	{
		uint32_t symHash;
		uint32_t addrMap;
		uint32_t thunkCount;
		uint32_t sizeOfThunk;
		uint16_t isectThunkTable;
		uint16_t padding;
		uint32_t offsetThunkTable;
		uint16_t sectionCount;
		uint16_t padding2;
	};

	// header of the hash tables used by the public and global symbol stream, based on GSIHashHdr defined here:
	// https://github.com/Microsoft/microsoft-pdb/blob/master/PDB/dbi/gsi.h#L62
	struct HashTableHeader
	{
		static inline constexpr const uint32_t Signature = 0xffffffffu;
		static inline constexpr const uint32_t Version = 0xeffe0000u + 19990810u;

		uint32_t signature;
		uint32_t version;
		uint32_t size;
		uint32_t bucketCount;
	};

	// hash record, based on HRFile defined here:
	// https://github.com/Microsoft/microsoft-pdb/blob/master/PDB/dbi/gsi.h#L8
	struct HashRecord
	{
		uint32_t offset;		// offset into the symbol record stream
		uint32_t cref;
	};


	namespace CodeView
	{
		// leaf types that are contained in other type or symbol records
		// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L937
		enum class PDB_NO_DISCARD NumericLeafType : uint16_t
		{
			LF_NUMERIC = 0x8000u,
			LF_CHAR = 0x8000u,
			LF_SHORT = 0x8001u,
			LF_USHORT = 0x8002u,
			LF_LONG = 0x8003u,
			LF_ULONG = 0x8004u,
			LF_REAL32 = 0x8005u,
			LF_REAL64 = 0x8006u,
			LF_REAL80 = 0x8007u,
			LF_REAL128 = 0x8008u,
			LF_QUADWORD = 0x8009u,
			LF_UQUADWORD = 0x800au,
			LF_REAL48 = 0x800bu,
			LF_COMPLEX32 = 0x800cu,
			LF_COMPLEX64 = 0x800du,
			LF_COMPLEX80 = 0x800eu,
			LF_COMPLEX128 = 0x800fu,
			LF_VARSTRING = 0x8010u,
			LF_OCTWORD = 0x8017u,
			LF_UOCTWORD = 0x8018u,
			LF_DECIMAL = 0x8019u,
			LF_DATE = 0x801au,
			LF_UTF8STRING = 0x801bu,
			LF_REAL16 = 0x801cu
		};
	}
}
