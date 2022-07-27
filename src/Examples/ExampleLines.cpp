// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleTimedScope.h"
#include "PDB_RawFile.h"
#include "PDB_DBIStream.h"
#include "PDB_InfoStream.h"

namespace
{
	struct Section
	{
		uint32_t index;
		uint32_t offset;
		size_t   lineIndex;
	};

	struct Line
	{
		uint32_t lineNumber;
		uint32_t codeSize;
		uint32_t fileChecksumsOffset;
		uint32_t namesFilenameOffset;
		PDB::CodeView::DBI::ChecksumKind checksumKind;
		uint8_t  checksumSize;
		uint8_t  checksum[32];
	};
}

void ExampleLines(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream, const PDB::InfoStream& infoStream)
{
	if (!infoStream.HasNamesStream())
	{
		printf("PDB has no '/names' stream for looking up filenames for lines, skipping \"Lines\" example.");
		return;
	}

	TimedScope total("\nRunning example \"Lines\"");

	// prepare names stream for grabbing file paths from lines
	TimedScope namesScope("Reading names stream");
	const PDB::NamesStream namesStream = infoStream.CreateNamesStream(rawPdbFile);
	namesScope.Done();

	// prepare the module info stream for grabbing function symbols from modules
	TimedScope moduleScope("Reading module info stream");
	const PDB::ModuleInfoStream moduleInfoStream = dbiStream.CreateModuleInfoStream(rawPdbFile);
	moduleScope.Done();

	// Keeping sections and lines separate, as sorting the smaller Section struct is 2x faster in release builds
	// than having all the fields in one big Line struct and sorting those.
	std::vector<Section> sections;
	std::vector<Line> lines;

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

			moduleLineStream.ForEachSection([&moduleLineStream, &namesStream, &moduleFileChecksumHeader, &lines, &sections](const PDB::CodeView::DBI::LineSection* lineSection)
			{
				if (lineSection->header.kind == PDB::CodeView::DBI::DebugSubsectionKind::S_LINES)
				{
					moduleLineStream.ForEachLinesBlock(lineSection, [&lines, &lineSection, &sections](const PDB::CodeView::DBI::LinesFileBlockHeader* linesBlockHeader)
					{
						const PDB::CodeView::DBI::Line& firstLine = linesBlockHeader->lines[0];

						const uint16_t sectionIndex  = lineSection->linesHeader.sectionIndex;
						const uint32_t sectionOffset = lineSection->linesHeader.sectionOffset;
						const uint32_t fileChecksumOffset = linesBlockHeader->fileChecksumOffset;

						sections.push_back({ sectionIndex, sectionOffset, lines.size() });
						lines.push_back({ firstLine.linenumStart, 0, fileChecksumOffset, 0, PDB::CodeView::DBI::ChecksumKind::None });

						uint32_t offset = 0;

						for(uint32_t i = 1, size = linesBlockHeader->numLines; i < size; ++i)
						{
							const PDB::CodeView::DBI::Line& line = linesBlockHeader->lines[i];

							sections.push_back({ sectionIndex, sectionOffset + line.offset, lines.size() });

							lines.back().codeSize = line.offset - offset;
							lines.push_back({ line.linenumStart, 0, fileChecksumOffset, 0, PDB::CodeView::DBI::ChecksumKind::None});

							offset = line.offset;
						}

						lines.back().codeSize = lineSection->linesHeader.codeSize - offset;

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
				else if (lineSection->header.kind == PDB::CodeView::DBI::DebugSubsectionKind::S_FILECHECKSUMS)
				{
					// how to read checksums
					moduleLineStream.ForEachFileChecksum(lineSection, [&namesStream](const PDB::CodeView::DBI::FileChecksumHeader* fileChecksumHeader)
					{
						const char* filename = namesStream.GetFilename(fileChecksumHeader->filenameOffset);
						(void)filename;
					});

					moduleFileChecksumHeader = &lineSection->checksumHeader;
				}
				else
				{
					PDB_ASSERT(false, "Header Kind 0x%X not handled", (uint32_t)lineSection->header.kind);
				}
			});
	
			// look up FileChecksumHeader data for each line added for in this module
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

		TimedScope sortScope("std::sort sections");

		std::sort(sections.begin(), sections.end(), [](const Section& lhs, const Section& rhs)
			{
				if (lhs.index == rhs.index)
				{
					return lhs.offset < rhs.offset;
				}

				return lhs.index < rhs.index;
			});

		sortScope.Done(sections.size());


#if 0
		// DIA2Dump style lines output
		static const char hexChars[17] = "0123456789ABCDEF";
		char checksumString[128];

		printf("*** LINES RAW PDB\n");

		const char* prevFilename = nullptr;

		for (const Section& section : sections)
		{
			const Line& line = lines[section.lineIndex];
			const char* filename = namesStream.GetFilename(line.namesFilenameOffset);
			
			const uint32_t rva = section.offset; // TODO: Figure out how to calc correctly.

			if (filename != prevFilename)
			{
				for (size_t i = 0, j = 0; i < line.checksumSize; ++i, j += 2)
				{
					checksumString[j] = hexChars[line.checksum[i] >> 4];
					checksumString[j + 1] = hexChars[line.checksum[i] & 0xF];
				}

				checksumString[line.checksumSize * 2] = '\0';

				printf("	line %u at [0x%08X][0x%04X:0x%08X], len = 0x%X %s (0x%02X: %s)\n",
					line.lineNumber, rva, section.index, section.offset, line.codeSize,
					filename, line.checksumKind, checksumString);
	
				prevFilename = filename;
			}
			else
			{
				printf("	line %u at [0x%08X][0x%04X:0x%08X], len = 0x%X\n",
					line.lineNumber, rva, section.index, section.offset, line.codeSize);
			}
		}
#endif
	}
}
