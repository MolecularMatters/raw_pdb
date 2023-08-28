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
		explicit ModuleLineStream(const RawFile& file, uint16_t streamIndex, uint32_t streamSize, size_t c13LineInfoOffset) PDB_NO_EXCEPT;

		PDB_DEFAULT_MOVE(ModuleLineStream);

		template <typename F>
		void ForEachSection(F&& functor) const PDB_NO_EXCEPT
		{
			size_t offset = m_c13LineInfoOffset;

			// read the line stream sections
			while (offset < m_stream.GetSize())
			{
				const CodeView::DBI::LineSection* section = m_stream.GetDataAtOffset<const CodeView::DBI::LineSection>(offset);

				functor(section);

				offset = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::DebugSubsectionHeader) + section->header.size, 4u);
			}
		}

		template <typename F>
		void ForEachLinesBlock(const CodeView::DBI::LineSection* section, F&& functor) const PDB_NO_EXCEPT
		{
			PDB_ASSERT(section->header.kind == CodeView::DBI::DebugSubsectionKind::S_LINES,
				"DebugSubsectionHeader::Kind %X != S_LINES (%X)", 
				static_cast<uint32_t>(section->header.kind), static_cast<uint32_t>(CodeView::DBI::DebugSubsectionKind::S_LINES));

			size_t offset = m_stream.GetPointerOffset(section);
			const size_t headerEnd = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::DebugSubsectionHeader) + section->header.size, 4u);

			offset = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::DebugSubsectionHeader) + sizeof(CodeView::DBI::LinesHeader), 4u);

			// read all blocks of lines
			while (offset < headerEnd)
			{
				const CodeView::DBI::LinesFileBlockHeader* linesBlockHeader = m_stream.GetDataAtOffset<const CodeView::DBI::LinesFileBlockHeader>(offset);
				const CodeView::DBI::Line* blockLines = m_stream.GetDataAtOffset<const CodeView::DBI::Line>(offset + sizeof(CodeView::DBI::LinesFileBlockHeader));

				const size_t blockColumnsOffset = sizeof(CodeView::DBI::LinesFileBlockHeader) + (linesBlockHeader->numLines * (sizeof(CodeView::DBI::Line)));
				const CodeView::DBI::Column* blockColumns = blockColumnsOffset < linesBlockHeader->size ? m_stream.GetDataAtOffset<const CodeView::DBI::Column>(offset) : nullptr;

				functor(linesBlockHeader, blockLines, blockColumns);

				offset = BitUtil::RoundUpToMultiple<size_t>(offset + linesBlockHeader->size, 4u);
			}

			PDB_ASSERT(offset == headerEnd, "Mismatch between offset %zu and header end %zu when reading lines blocks", offset, headerEnd);
		}

		template <typename F>
		void ForEachFileChecksum(const CodeView::DBI::LineSection* section, F&& functor) const PDB_NO_EXCEPT
		{
			PDB_ASSERT(section->header.kind == CodeView::DBI::DebugSubsectionKind::S_FILECHECKSUMS,
				"DebugSubsectionHeader::Kind %X != S_FILECHECKSUMS (%X)", 
				static_cast<uint32_t>(section->header.kind), static_cast<uint32_t>(CodeView::DBI::DebugSubsectionKind::S_FILECHECKSUMS));

			size_t offset = m_stream.GetPointerOffset(section);
			const size_t headerEnd = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::DebugSubsectionHeader) + section->header.size, 4u);

			offset = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::DebugSubsectionHeader), 4u);

			// read all file checksums
			while (offset < headerEnd)
			{
				const CodeView::DBI::FileChecksumHeader* fileChecksumHeader = m_stream.GetDataAtOffset<const CodeView::DBI::FileChecksumHeader>(offset);

				functor(fileChecksumHeader);

				offset = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::FileChecksumHeader) + fileChecksumHeader->checksumSize, 4u);
			}

			PDB_ASSERT(offset == headerEnd, "Mismatch between offset %zu and header end %zu when reading file checksums", offset, headerEnd);
		}

		template <typename F>
		void ForEachInlineeSourceLine(const CodeView::DBI::LineSection* section, F&& functor) const PDB_NO_EXCEPT
		{
			PDB_ASSERT(section->header.kind == CodeView::DBI::DebugSubsectionKind::S_INLINEELINES,
				"DebugSubsectionHeader::Kind %X != S_INLINEELINES (%X)", 
				static_cast<uint32_t>(section->header.kind), static_cast<uint32_t>(CodeView::DBI::DebugSubsectionKind::S_INLINEELINES));

			PDB_ASSERT(section->inlineeHeader.kind == CodeView::DBI::InlineeSourceLineKind::Signature,
				"InlineeSourceLineKind %X != :InlineeSourceLineKind::Signature (%X)", static_cast<uint32_t>(section->header.kind), static_cast<uint32_t>(CodeView::DBI::InlineeSourceLineKind::Signature));

			size_t offset = m_stream.GetPointerOffset(section);
			const size_t headerEnd = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::DebugSubsectionHeader) + section->header.size, 4u);

			offset = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::DebugSubsectionHeader) + sizeof(CodeView::DBI::InlineeSourceLineHeader), 4u);

			// read all file checksums
			while (offset < headerEnd)
			{
				const CodeView::DBI::InlineeSourceLine* inlineeSourceLine = m_stream.GetDataAtOffset<const CodeView::DBI::InlineeSourceLine>(offset);

				functor(inlineeSourceLine);

				offset = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::InlineeSourceLine), 4u);
			}
		}

		template <typename F>
		void ForEachInlineeSourceLineEx(const CodeView::DBI::LineSection* section, F&& functor) const PDB_NO_EXCEPT
		{
			PDB_ASSERT(section->header.kind == CodeView::DBI::DebugSubsectionKind::S_INLINEELINES,
				"DebugSubsectionHeader::Kind %X != S_INLINEELINES (%X)", static_cast<uint32_t>(section->header.kind), static_cast<uint32_t>(CodeView::DBI::DebugSubsectionKind::S_INLINEELINES));

			PDB_ASSERT(section->inlineeHeader.kind == CodeView::DBI::InlineeSourceLineKind::SignatureEx,
				"InlineeSourceLineKind %X != :InlineeSourceLineKind::SignatureEx (%X)", static_cast<uint32_t>(section->header.kind), static_cast<uint32_t>(CodeView::DBI::InlineeSourceLineKind::SignatureEx));

			size_t offset = m_stream.GetPointerOffset(section);
			const size_t headerEnd = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::DebugSubsectionHeader) + section->header.size, 4u);

			offset = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::DebugSubsectionHeader) + sizeof(CodeView::DBI::InlineeSourceLineHeader), 4u);

			// read all file checksums
			while (offset < headerEnd)
			{
				const CodeView::DBI::InlineeSourceLineEx* inlineeSourceLineEx = m_stream.GetDataAtOffset<const CodeView::DBI::InlineeSourceLineEx>(offset);

				functor(inlineeSourceLineEx);

				offset = BitUtil::RoundUpToMultiple<size_t>(offset + sizeof(CodeView::DBI::InlineeSourceLineEx) + (inlineeSourceLineEx->extraLines * sizeof(uint32_t)), 4u);
			}
		}
	private:
		CoalescedMSFStream m_stream;
		size_t m_c13LineInfoOffset;

		PDB_DISABLE_COPY(ModuleLineStream);
	};
}
