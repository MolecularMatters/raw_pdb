// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleTimedScope.h"
#include "PDB_RawFile.h"
#include "PDB_DBIStream.h"
#include "PDB_NamesStream.h"

namespace
{
	struct Line
	{
		uint32_t byteOffset;
		uint32_t lineNumber;
		uint16_t sectionIndex;
		uint32_t sectionOffset;
		uint32_t fileChecksumsOffset;
		uint32_t namesFilenameOffset;
	};
}

void ExampleLines(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream, const PDB::NamesStream& namesStream)
{
	TimedScope total("\nRunning example \"Lines\"");

	// in order to keep the example easy to understand, we load the PDB data serially.
	// note that this can be improved a lot by reading streams concurrently.

	// prepare the module info stream for grabbing function symbols from modules
	TimedScope moduleScope("Reading module info stream");
	const PDB::ModuleInfoStream moduleInfoStream = dbiStream.CreateModuleInfoStream(rawPdbFile);
	moduleScope.Done();

	std::vector<Line> lines;
	lines.reserve(1024);

	{
		TimedScope scope("Storing lines from modules");

		const PDB::ArrayView<PDB::ModuleInfoStream::Module> modules = moduleInfoStream.GetModules();

		for (const PDB::ModuleInfoStream::Module& module : modules)
		{
			if (!module.HasLineStream())
			{
				continue;
			}

			const PDB::ModuleLineStream moduleLineStream = module.CreateLineStream(rawPdbFile);

			const size_t moduleLinesStartIndex = lines.size();
			const PDB::CodeView::DBI::FileChecksumHeader* moduleFileChecksumHeader = nullptr;

			moduleLineStream.ForEachSection([&moduleLineStream, &namesStream, &moduleFileChecksumHeader, &lines](const PDB::CodeView::DBI::LineSection* section)
			{
				if (section->header.kind == PDB::CodeView::DBI::DebugSubsectionKind::S_LINES)
				{
					moduleLineStream.ForEachLinesBlock(section, [&lines, &section](const PDB::CodeView::DBI::LinesFileBlockHeader* linesBlockHeader)
					{
						for(uint32_t i = 0, size = linesBlockHeader->numLines; i < size; ++i)
						{
							const PDB::CodeView::DBI::Line& line = linesBlockHeader->lines[i];

							lines.push_back({ line.offset, line.linenumStart, 
												section->linesHeader.sectionIndex, section->linesHeader.sectionOffset,
												linesBlockHeader->fileChecksumOffset, 0});
						}

						if (!linesBlockHeader->HasColumns())
						{
							return;
						}

						const PDB::CodeView::DBI::Column* columns = linesBlockHeader->GetColumns();

						for (uint32_t i = 0, size = linesBlockHeader->numLines; i < size; ++i)
						{
							const PDB::CodeView::DBI::Column& column = columns[i];
							(void)column;
						}
					});
				}
				else if (section->header.kind == PDB::CodeView::DBI::DebugSubsectionKind::S_FILECHECKSUMS)
				{
					// how to read checksums
					moduleLineStream.ForEachFileChecksum(section, [&namesStream](const PDB::CodeView::DBI::FileChecksumHeader* fileChecksumHeader)
					{
						const char* filename = namesStream.GetFilename(fileChecksumHeader->filenameOffset);
						(void)filename;
					});

					moduleFileChecksumHeader = &section->checksumHeader;
				}
				else
				{
					PDB_ASSERT(false, "Header Kind 0x%X not handled", (uint32_t)section->header.kind);
				}
			});
	
			// look up and store NamesStream filename offset for all lines added in this module
			for (size_t i = moduleLinesStartIndex, size = lines.size(); i < size; ++i)
			{
				Line& line = lines[i];

				// look up FileChecksumHeader which contains the NamesStream filename offset.
				const PDB::CodeView::DBI::FileChecksumHeader* checksumHeader = PDB::ModuleLineStream::GetFileChecksumHeaderAtOffset(moduleFileChecksumHeader, line.fileChecksumsOffset);

				PDB_ASSERT(checksumHeader->checksumKind >= PDB::CodeView::DBI::ChecksumKind::None && 
							checksumHeader->checksumKind <= PDB::CodeView::DBI::ChecksumKind::SHA256,
							"Invalid checksum kind %u", checksumHeader->checksumKind);

				// store NamesStream filename offset.
				line.namesFilenameOffset = checksumHeader->filenameOffset;
			}
		}

		scope.Done(modules.GetLength());

		TimedScope sortScope("std::sort lines");

		std::sort(lines.begin(), lines.end(), [](const Line& lhs, const Line& rhs)
		{
			if (lhs.sectionIndex == rhs.sectionIndex)
			{
				return lhs.sectionOffset < rhs.sectionOffset;
			}

			return lhs.sectionIndex < rhs.sectionIndex;
		});

		sortScope.Done(lines.size());
	}
}
