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
		uint32_t lineNumber;
		uint16_t sectionIndex;
		uint32_t sectionOffset;
		uint32_t codeSize;
		uint32_t fileChecksumsOffset;
		uint32_t namesFilenameOffset;
		PDB::CodeView::DBI::ChecksumKind checksumKind;
		uint8_t  checksumSize;
		uint8_t  checksum[32];
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
						const PDB::CodeView::DBI::Line& firstLine = linesBlockHeader->lines[0];

						lines.push_back({ firstLine.linenumStart,
								section->linesHeader.sectionIndex, section->linesHeader.sectionOffset + firstLine.offset, 0,
								linesBlockHeader->fileChecksumOffset, 0, PDB::CodeView::DBI::ChecksumKind::None });

						uint32_t offset = 0;

						for(uint32_t i = 1, size = linesBlockHeader->numLines; i < size; ++i)
						{
							const PDB::CodeView::DBI::Line& line = linesBlockHeader->lines[i];

							lines.back().codeSize = line.offset - offset;

							lines.push_back({ line.linenumStart,
												section->linesHeader.sectionIndex, section->linesHeader.sectionOffset + line.offset, 0,
												linesBlockHeader->fileChecksumOffset, 0, PDB::CodeView::DBI::ChecksumKind::None});

							offset = line.offset;
						}

						lines.back().codeSize = section->linesHeader.codeSize - offset;

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
	
			// look up FileChecksumHeader each line added in this module
			for (size_t i = moduleLinesStartIndex, size = lines.size(); i < size; ++i)
			{
				Line& line = lines[i];

				// look up FileChecksumHeader
				const PDB::CodeView::DBI::FileChecksumHeader* checksumHeader = PDB::ModuleLineStream::GetFileChecksumHeaderAtOffset(moduleFileChecksumHeader, line.fileChecksumsOffset);

				PDB_ASSERT(checksumHeader->checksumKind >= PDB::CodeView::DBI::ChecksumKind::None && 
							checksumHeader->checksumKind <= PDB::CodeView::DBI::ChecksumKind::SHA256,
							"Invalid checksum kind %u", checksumHeader->checksumKind);

				// store FileChecksumHeader values
				line.namesFilenameOffset = checksumHeader->filenameOffset;
				line.checksumKind = checksumHeader->checksumKind;
				line.checksumSize = checksumHeader->checksumSize;
				std::memcpy(line.checksum, checksumHeader->checksum, checksumHeader->checksumSize);
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

#if 0
		// DIA2Dump style lines output
		static const char hexChars[17] = "0123456789ABCDEF";
		char checksumString[128];

		printf("*** LINES RAW PDB\n")

		for (const Line& line : lines)
		{
			const char* filename = namesStream.GetFilename(line.namesFilenameOffset);
			
			for (size_t i = 0, j = 0; i < line.checksumSize; ++i, j+=2)
			{
				checksumString[j]   = hexChars[line.checksum[i] >> 4];
				checksumString[j+1] = hexChars[line.checksum[i] & 0xF];
			}

			checksumString[line.checksumSize * 2] = '\0';

			printf("	line %u at [0x%04X:0x%08X], len = 0x%X %s (0x%02X: %s)\n",
				line.lineNumber, line.sectionIndex, line.sectionOffset, line.codeSize,
				filename, line.checksumKind, checksumString);
		}
#endif
	}
}
