// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleTimedScope.h"
#include "PDB_RawFile.h"
#include "PDB_DBIStream.h"


namespace
{
	// we don't have to store std::string in the contributions, since all the data is memory-mapped anyway.
	// we do it in this example to ensure that we don't "cheat" when reading the PDB file. memory-mapped data will only
	// be faulted into the process once it's touched, so actually copying the string data makes us touch the needed data,
	// giving us a real performance measurement.
	struct Contribution
	{
		std::string objectFile;
		uint32_t rva;
		uint32_t size;
	};
}


void ExampleContributions(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream);
void ExampleContributions(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream)
{
	TimedScope total("\nRunning example \"Contributions\"");

	// in order to keep the example easy to understand, we load the PDB data serially.
	// note that this can be improved a lot by reading streams concurrently.

	// prepare the image section stream first. it is needed for converting section + offset into an RVA
	TimedScope sectionScope("Reading image section stream");
	const PDB::ImageSectionStream imageSectionStream = dbiStream.CreateImageSectionStream(rawPdbFile);
	sectionScope.Done();


	// prepare the module info stream for matching contributions against files
	TimedScope moduleScope("Reading module info stream");
	const PDB::ModuleInfoStream moduleInfoStream = dbiStream.CreateModuleInfoStream(rawPdbFile);
	moduleScope.Done();


	// read contribution stream
	TimedScope contributionScope("Reading section contribution stream");
	const PDB::SectionContributionStream sectionContributionStream = dbiStream.CreateSectionContributionStream(rawPdbFile);
	contributionScope.Done();

	std::vector<Contribution> contributions;
	{
		TimedScope scope("Storing contributions");

		const PDB::ArrayView<PDB::DBI::SectionContribution> sectionContributions = sectionContributionStream.GetContributions();
		const size_t count = sectionContributions.GetLength();

		contributions.reserve(count);

		for (const PDB::DBI::SectionContribution& contribution : sectionContributions)
		{
			const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(contribution.section, contribution.offset);
			if (rva == 0u)
			{
				printf("Contribution has invalid RVA\n");
				continue;
			}

			const PDB::ModuleInfoStream::Module& module = moduleInfoStream.GetModule(contribution.moduleIndex);

			contributions.push_back(Contribution { module.GetName().Decay(), rva, contribution.size });
		}
		
		scope.Done(count);
	}

	TimedScope sortScope("std::sort contributions");
	std::sort(contributions.begin(), contributions.end(), [](const Contribution& lhs, const Contribution& rhs)
	{
		return lhs.size > rhs.size;
	});
	sortScope.Done();

	total.Done();

	// log the 20 largest contributions
	{
		printf("20 largest contributions:\n");

		const size_t countToShow = std::min<size_t>(20ul, contributions.size());
		for (size_t i = 0u; i < countToShow; ++i)
		{
			const Contribution& contribution = contributions[i];
			printf("%zu: %u bytes from %s\n", i + 1u, contribution.size, contribution.objectFile.c_str());
		}
	}
}
