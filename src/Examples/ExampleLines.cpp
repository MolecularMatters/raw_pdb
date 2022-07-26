// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleTimedScope.h"
#include "PDB_RawFile.h"
#include "PDB_DBIStream.h"

void ExampleLines(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream)
{
	TimedScope total("\nRunning example \"Lines\"");

	// in order to keep the example easy to understand, we load the PDB data serially.
	// note that this can be improved a lot by reading streams concurrently.

	// prepare the image section stream first. it is needed for converting section + offset into an RVA
	TimedScope sectionScope("Reading image section stream");
	const PDB::ImageSectionStream imageSectionStream = dbiStream.CreateImageSectionStream(rawPdbFile);
	sectionScope.Done();


	// prepare the module info stream for grabbing function symbols from modules
	TimedScope moduleScope("Reading module info stream");
	const PDB::ModuleInfoStream moduleInfoStream = dbiStream.CreateModuleInfoStream(rawPdbFile);
	moduleScope.Done();

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

			moduleLineStream.ForEachSection([&moduleLineStream](const PDB::CodeView::DBI::LineSection* section)
			{
				if (section->header.kind == PDB::CodeView::DBI::DebugSubsectionKind::S_LINES)
				{
					moduleLineStream.ForEachLinesBlock(section, [](const PDB::CodeView::DBI::LinesFileBlockHeader* linesBlockHeader)
					{
						for(uint32_t i = 0, size = linesBlockHeader->numLines; i < size; ++i)
						{
							const PDB::CodeView::DBI::Line& line = linesBlockHeader->lines[i];
							(void)line;
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
					moduleLineStream.ForEachFileChecksum(section, [](const PDB::CodeView::DBI::FileChecksumHeader* fileChecksumHeader)
					{
						(void)fileChecksumHeader;

					});

				}
			});

		}

		scope.Done(modules.GetLength());
	}
}
