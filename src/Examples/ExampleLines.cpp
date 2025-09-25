// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleTimedScope.h"
#include "Foundation/PDB_PointerUtil.h"
#include "PDB_RawFile.h"
#include "PDB_DBIStream.h"
#include "PDB_InfoStream.h"

#include <cstring>

namespace
{
	struct Section
	{
		uint16_t index;
		uint32_t offset;
		size_t   lineIndex;
	};

	struct Filename
	{
		uint32_t fileChecksumOffset;
		uint32_t namesFilenameOffset;
		PDB::CodeView::DBI::ChecksumKind checksumKind;
		uint8_t  checksumSize;
		uint8_t  checksum[32];
	};

	struct Line
	{
		uint32_t lineNumber;
		uint32_t codeSize;
		size_t   filenameIndex;
	};
}

void ExampleLines(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream, const PDB::InfoStream& infoStream);
void ExampleLines(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream, const PDB::InfoStream& infoStream)
{
	if (!infoStream.HasNamesStream())
	{
		printf("PDB has no '/names' stream for looking up filenames for lines, skipping \"Lines\" example.");
		return;
	}

	TimedScope total("\nRunning example \"Lines\"");

	// prepare the image section stream first. it is needed for converting section + offset into an RVA
	TimedScope sectionScope("Reading image section stream");
	const PDB::ImageSectionStream imageSectionStream = dbiStream.CreateImageSectionStream(rawPdbFile);
	sectionScope.Done();

	// prepare the module info stream for grabbing function symbols from modules
	TimedScope moduleScope("Reading module info stream");
	const PDB::ModuleInfoStream moduleInfoStream = dbiStream.CreateModuleInfoStream(rawPdbFile);
	moduleScope.Done();

	// prepare names stream for grabbing file paths from lines
	TimedScope namesScope("Reading names stream");
	const PDB::NamesStream namesStream = infoStream.CreateNamesStream(rawPdbFile);
	namesScope.Done();

	// keeping sections and lines separate, as sorting the smaller Section struct is 2x faster in release builds
	// than having all the fields in one big Line struct and sorting those.
	std::vector<Section> sections;
	std::vector<Filename> filenames;
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

			const size_t moduleFilenamesStartIndex = filenames.size();
			const PDB::CodeView::DBI::FileChecksumHeader* moduleFileChecksumHeader = nullptr;

			moduleLineStream.ForEachSection([&moduleLineStream, &namesStream, &moduleFileChecksumHeader, &sections, &filenames, &lines](const PDB::CodeView::DBI::LineSection* lineSection)
			{
				if (lineSection->header.kind == PDB::CodeView::DBI::DebugSubsectionKind::S_LINES)
				{
					moduleLineStream.ForEachLinesBlock(lineSection, 
					[&lineSection, &sections, &filenames, &lines](const PDB::CodeView::DBI::LinesFileBlockHeader* linesBlockHeader, const PDB::CodeView::DBI::Line* blocklines, const PDB::CodeView::DBI::Column* blockColumns)
					{
						if (linesBlockHeader->numLines == 0)
						{
							return;
						}

						const PDB::CodeView::DBI::Line& firstLine = blocklines[0];

						const uint16_t sectionIndex  = lineSection->linesHeader.sectionIndex;
						const uint32_t sectionOffset = lineSection->linesHeader.sectionOffset;
						const uint32_t fileChecksumOffset = linesBlockHeader->fileChecksumOffset;

						const size_t filenameIndex = filenames.size();

						// there will be duplicate filenames for any real world pdb. 
						// ideally the filenames would be stored in a map with the filename or checksum as the key.
						// but that would complicate the logic in this example and therefore just use a vector to make it easier to understand.
						filenames.push_back({ fileChecksumOffset, 0, PDB::CodeView::DBI::ChecksumKind::None, 0, {0} });

						sections.push_back({ sectionIndex, sectionOffset, lines.size() });

						// initially set code size of first line to 0, will be updated in loop below.
						lines.push_back({ firstLine.linenumStart, 0, filenameIndex });

						for(uint32_t i = 1, size = linesBlockHeader->numLines; i < size; ++i)
						{
							const PDB::CodeView::DBI::Line& line = blocklines[i];

							// calculate code size of previous line by using the current line offset.
							lines.back().codeSize = line.offset - blocklines[i-1].offset;

							sections.push_back({ sectionIndex, sectionOffset + line.offset, lines.size() });
							lines.push_back({ line.linenumStart, 0, filenameIndex });
						}

						// calc code size of last line
						lines.back().codeSize = lineSection->linesHeader.codeSize - blocklines[linesBlockHeader->numLines-1].offset;

						// columns are optional
						if (blockColumns == nullptr)
						{
							return;
						}

						for (uint32_t i = 0, size = linesBlockHeader->numLines; i < size; ++i)
						{
							const PDB::CodeView::DBI::Column& column = blockColumns[i];
							(void)column;
						}
					});
				}
				else if (lineSection->header.kind == PDB::CodeView::DBI::DebugSubsectionKind::S_FILECHECKSUMS)
				{
					// how to read checksums and their filenames from the Names Stream
					moduleLineStream.ForEachFileChecksum(lineSection, [&namesStream](const PDB::CodeView::DBI::FileChecksumHeader* fileChecksumHeader)
					{
						const char* filename = namesStream.GetFilename(fileChecksumHeader->filenameOffset);
						(void)filename;
					});

					// store the checksum header for the module, as there might be more lines after the checksums.
					// so lines will get their checksum header values assigned after processing all line sections in the module.
					PDB_ASSERT(moduleFileChecksumHeader == nullptr, "Module File Checksum Header already set");
					moduleFileChecksumHeader = &lineSection->checksumHeader;
				}
				else if (lineSection->header.kind == PDB::CodeView::DBI::DebugSubsectionKind::S_INLINEELINES)
				{
					if (lineSection->inlineeHeader.kind == PDB::CodeView::DBI::InlineeSourceLineKind::Signature)
					{
						moduleLineStream.ForEachInlineeSourceLine(lineSection, [](const PDB::CodeView::DBI::InlineeSourceLine* inlineeSourceLine)
						{
							(void)inlineeSourceLine;

						});
					}
					else
					{
						moduleLineStream.ForEachInlineeSourceLineEx(lineSection, [](const PDB::CodeView::DBI::InlineeSourceLineEx* inlineeSourceLineEx)
						{
							for (uint32_t i = 0; i < inlineeSourceLineEx->extraLines; ++i)
							{
								const uint32_t checksumOffset = inlineeSourceLineEx->extrafileChecksumOffsets[i];
								(void)checksumOffset;
							}
						});
					}
				}
				else
				{
					PDB_ASSERT(false, "Line Section kind 0x%X not handled", static_cast<uint32_t>(lineSection->header.kind));
				}
			});
	
			// assign checksum values for each filename added in this module
			for (size_t i = moduleFilenamesStartIndex, size = filenames.size(); i < size; ++i)
			{
				Filename& filename = filenames[i];

				// look up the filename's checksum header in the module's checksums section
				const PDB::CodeView::DBI::FileChecksumHeader* checksumHeader = PDB::Pointer::Offset<const PDB::CodeView::DBI::FileChecksumHeader*>(moduleFileChecksumHeader, filename.fileChecksumOffset);

				PDB_ASSERT(checksumHeader->checksumKind >= PDB::CodeView::DBI::ChecksumKind::None && 
							checksumHeader->checksumKind <= PDB::CodeView::DBI::ChecksumKind::SHA256,
							"Invalid checksum kind %u", static_cast<uint16_t>(checksumHeader->checksumKind));

				// store checksum values in filname struct
				filename.namesFilenameOffset = checksumHeader->filenameOffset;
				filename.checksumKind = checksumHeader->checksumKind;
				filename.checksumSize = checksumHeader->checksumSize;
				std::memcpy(filename.checksum, checksumHeader->checksum, checksumHeader->checksumSize);
			}
		}

		scope.Done(modules.GetLength());

		TimedScope sortScope("std::sort sections");

		// sort sections, so we can iterate over lines by address order.
		std::sort(sections.begin(), sections.end(), [](const Section& lhs, const Section& rhs)
		{
			if (lhs.index == rhs.index)
			{
				return lhs.offset < rhs.offset;
			}

			return lhs.index < rhs.index;
		});

		sortScope.Done(sections.size());

// Disabled by default, as it will print a lot of lines for large PDBs :-)
#if 0
		// DIA2Dump style lines output
		static const char hexChars[17] = "0123456789ABCDEF";
		char checksumString[128];

		printf("*** LINES RAW PDB\n");

		const char* prevFilename = nullptr;

		for (const Section& section : sections)
		{
			const Line& line = lines[section.lineIndex];
			const Filename& lineFilename = filenames[line.filenameIndex];

			const char* filename = namesStream.GetFilename(lineFilename.namesFilenameOffset);
			
			const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(section.index, section.offset);

			// only print filename for a line if it is different from the previous one.
			if (filename != prevFilename)
			{
				for (size_t i = 0, j = 0; i < lineFilename.checksumSize; i++, j+=2)
				{
					checksumString[j]   = hexChars[lineFilename.checksum[i] >> 4];
					checksumString[j+1] = hexChars[lineFilename.checksum[i] & 0xF];
				}

				checksumString[lineFilename.checksumSize * 2] = '\0';

				printf("	line %u at [0x%08X][0x%04X:0x%08X], len = 0x%X %s (0x%02X: %s)\n",
					line.lineNumber, rva, section.index, section.offset, line.codeSize,
					filename, static_cast<uint32_t>(lineFilename.checksumKind), checksumString);
	
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
