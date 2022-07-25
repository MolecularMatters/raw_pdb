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

			moduleLineStream.ForEachLineBlock([](const PDB::CodeView::DBI::Line* lines, int32_t numLines)
			{
					(void)lines;
					(void)numLines;
			});

		}

		scope.Done(modules.GetLength());
	}
}