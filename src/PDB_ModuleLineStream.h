// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "Foundation/PDB_Macros.h"
#include "Foundation/PDB_BitUtil.h"
#include "PDB_DBITypes.h"
#include "PDB_Util.h"
#include "PDB_CoalescedMSFStream.h"


namespace PDB
{
	class RawFile;

	class PDB_NO_DISCARD ModuleLineStream
	{
	public:
		ModuleLineStream(void) PDB_NO_EXCEPT;
		explicit ModuleLineStream(const RawFile& file, uint16_t streamIndex, uint32_t symbolSize, size_t lineInfoOffset) PDB_NO_EXCEPT;

		PDB_DEFAULT_MOVE(ModuleLineStream);

		// Iterates all lines in the stream.
		template <typename F>
		void ForEachLineBlock(F&& functor) const PDB_NO_EXCEPT
		{
			size_t offset = m_lineInfoOffset;

			// parse the line stream entries
			while (offset < m_stream.GetSize())
			{
				const CodeView::DBI::DebugSubsectionHeader* header = m_stream.GetDataAtOffset<const CodeView::DBI::DebugSubsectionHeader>(offset);

				// Subsection is not lines, skip to next
				if (header->kind != CodeView::DBI::DebugSubsectionKind::S_LINES)
				{
					offset = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::DebugSubsectionHeader) + header->size, 4u);
					continue;
				}

				offset = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::DebugSubsectionHeader), 4u);

				const CodeView::DBI::LinesHeader* linesHeader = m_stream.GetDataAtOffset<const CodeView::DBI::LinesHeader>(offset);
				const CodeView::DBI::LinesFileBlockHeader* linesBlockHeader = m_stream.GetDataAtOffset<const CodeView::DBI::LinesFileBlockHeader>(offset + sizeof(CodeView::DBI::LinesHeader));
				const CodeView::DBI::Line* lines = m_stream.GetDataAtOffset<const CodeView::DBI::Line>(offset + sizeof(CodeView::DBI::LinesHeader) + sizeof(CodeView::DBI::LinesFileBlockHeader));

				(void)functor;
				(void)linesHeader;
				(void)linesBlockHeader;
				(void)lines;

				offset = BitUtil::RoundUpToMultiple<size_t>(offset + header->size, 4u);
			}
		}

	private:
		CoalescedMSFStream m_stream;
		size_t m_lineInfoOffset;

		PDB_DISABLE_COPY(ModuleLineStream);
	};
}
