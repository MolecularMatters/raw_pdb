// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleTimedScope.h"
#include "ExampleTypeTable.h"
#include "PDB_RawFile.h"
#include "PDB_InfoStream.h"
#include "PDB_IPIStream.h"
#include "PDB_TPIStream.h"

static std::string GetTypeNameIPI(const TypeTable& typeTable, uint32_t typeIndex)
{
	// Defined in ExampleTypes.cpp
	extern std::string GetTypeName(const TypeTable & typeTable, uint32_t typeIndex);

	std::string typeName = GetTypeName(typeTable, typeIndex);

	// Remove any '%s' substring used to insert a variable/field name.
	const uint64_t markerPos = typeName.find("%s");
	if (markerPos != typeName.npos)
	{
		typeName.erase(markerPos, 2);
	}

	return typeName;
}

void ExampleIPI(const PDB::RawFile& rawPdbFile, const PDB::InfoStream& infoStream, const PDB::TPIStream& tpiStream, const PDB::IPIStream& ipiStream);

void ExampleIPI(const PDB::RawFile& rawPdbFile, const PDB::InfoStream& infoStream, const PDB::TPIStream& tpiStream, const PDB::IPIStream& ipiStream)
{
	if (!infoStream.HasIPIStream())
	{
		return;
	}

	TimedScope total("\nRunning example \"IPI\"");

	TimedScope typeTableScope("Create TypeTable");
	TypeTable typeTable(tpiStream);
	typeTableScope.Done();

	// prepare names stream for grabbing file paths from lines
	TimedScope namesScope("Reading names stream");
	const PDB::NamesStream namesStream = infoStream.CreateNamesStream(rawPdbFile);
	namesScope.Done();

	const uint32_t firstTypeIndex = ipiStream.GetFirstTypeIndex();

	PDB::ArrayView<const PDB::CodeView::IPI::Record*> records = ipiStream.GetTypeRecords();

	std::vector<const char*> strings;

	strings.resize(records.GetLength(), nullptr);

	size_t index = 0;

	for (const PDB::CodeView::IPI::Record* record : records)
	{
		const PDB::CodeView::IPI::RecordHeader& header = record->header;

		if (header.kind == PDB::CodeView::IPI::TypeRecordKind::LF_STRING_ID)
		{
			strings[index] = record->data.LF_STRING_ID.name;
		}

		index++;
	}

	uint32_t identifier = firstTypeIndex;

	std::string typeName, parentTypeName;

	printf("\n --- IPI Records ---\n\n");

	for(const PDB::CodeView::IPI::Record* record : records)
	{
		const PDB::CodeView::IPI::RecordHeader& header = record->header;

		if (header.kind == PDB::CodeView::IPI::TypeRecordKind::LF_FUNC_ID)
		{ 
			typeName = GetTypeNameIPI(typeTable, record->data.LF_FUNC_ID.typeIndex);

			printf("Kind: 'LF_FUNC_ID' Size: %i ID: %u\n", header.size, identifier);
			printf(" Scope ID: %u\n Type: '%s'\n Name: '%s'\n\n", 
				record->data.LF_FUNC_ID.scopeId, 
				typeName.c_str(), 
				record->data.LF_FUNC_ID.name);

		}
		else if (header.kind == PDB::CodeView::IPI::TypeRecordKind::LF_MFUNC_ID)
		{
			typeName = GetTypeNameIPI(typeTable, record->data.LF_MFUNC_ID.typeIndex);
			parentTypeName = GetTypeNameIPI(typeTable, record->data.LF_MFUNC_ID.parentTypeIndex);

			printf("Kind: 'LF_MFUNC_ID' Size: %i ID: %u\n", header.size, identifier);
			printf(" Parent Type: '%s'\n Type: '%s'\n Name: '%s'\n\n",
				parentTypeName.c_str(),
				typeName.c_str(),
				record->data.LF_MFUNC_ID.name);

		}
		else if (header.kind == PDB::CodeView::IPI::TypeRecordKind::LF_BUILDINFO)
		{
			printf("Kind: 'LF_BUILDINFO' Size: %u ID: %u\n", header.size, identifier);

			if (record->data.LF_BUILDINFO.count == 0)
			{
				continue;
			}

			printf("Strings: '%s'", strings[record->data.LF_BUILDINFO.typeIndices[0] - firstTypeIndex]);

			for (uint32_t i = 1, size = record->data.LF_BUILDINFO.count; i < size; ++i)
			{
				const uint32_t stringIndex = record->data.LF_BUILDINFO.typeIndices[i];

				if (stringIndex == 0)
				{
					printf(", ''");
				}
				else
				{
					printf(", '%s'", strings[stringIndex - firstTypeIndex]);
				}
			}

			printf("\n\n");
		}
		else if (header.kind == PDB::CodeView::IPI::TypeRecordKind::LF_SUBSTR_LIST)
		{
			printf("Kind: 'LF_SUBSTR_LIST' Size: %u ID: %u\n", header.size, identifier);

			if (record->data.LF_SUBSTR_LIST.count == 0)
			{
				continue;
			}

			printf(" Strings: '%s'", strings[record->data.LF_SUBSTR_LIST.typeIndices[0] - firstTypeIndex]);

			for (uint32_t i = 1, size = record->data.LF_SUBSTR_LIST.count; i < size; ++i)
			{
				const uint32_t stringIndex = record->data.LF_SUBSTR_LIST.typeIndices[i];

				if (stringIndex == 0)
				{
					printf(", ''");
				}
				else
				{
					printf(", '%s'", strings[stringIndex - firstTypeIndex]);
				}
			}

			printf("\n\n");
		}
		else if (header.kind == PDB::CodeView::IPI::TypeRecordKind::LF_STRING_ID)
		{
			printf("Kind: 'LF_STRING_ID' Size: %u ID: %u\n", header.size, identifier);

			printf(" Substring ID: %u\n Name: '%s'\n\n", record->data.LF_STRING_ID.id, record->data.LF_STRING_ID.name);
		}
		else if (header.kind == PDB::CodeView::IPI::TypeRecordKind::LF_UDT_SRC_LINE)
		{
			typeName = GetTypeNameIPI(typeTable, record->data.LF_UDT_SRC_LINE.typeIndex);

			const uint32_t stringIndex = record->data.LF_UDT_SRC_LINE.stringIndex;

			printf("Kind: 'LF_UDT_SRC_LINE' Size: %u ID: %u\n", header.size, identifier);

			printf(" Type: '%s'\n Source Path: %s\n Line: %u\n\n", 
				typeName.c_str(),
				strings[stringIndex - firstTypeIndex],
				record->data.LF_UDT_SRC_LINE.line);
		}
		else if (header.kind == PDB::CodeView::IPI::TypeRecordKind::LF_UDT_MOD_SRC_LINE)
		{
			typeName = GetTypeNameIPI(typeTable, record->data.LF_UDT_MOD_SRC_LINE.typeIndex);

			const char* string = namesStream.GetFilename(record->data.LF_UDT_MOD_SRC_LINE.stringIndex);

			printf("Kind: 'LF_UDT_SRC_LINE' Size: %u ID: %u\n", header.size, identifier);

			printf(" Type: '%s'\n Source Path: %s\n Line: %u\n Module Index: %u\n\n",
				typeName.c_str(),
				string,
				record->data.LF_UDT_MOD_SRC_LINE.line,
				record->data.LF_UDT_MOD_SRC_LINE.moduleIndex);
		}
		else
		{
			printf("Kind: 0x%X Size: %u ID: %u\n\n", static_cast<uint32_t>(header.kind), header.size, identifier);
		}

		identifier++;
	}
}
