//
// Expansion Hunter
// Copyright (c) 2018 Illumina, Inc.
//
// Author: Egor Dolzhenko <edolzhenko@illumina.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#pragma once

#include <list>
#include <string>

#include "graphalign/GappedAligner.hh"
#include "graphalign/GraphAlignment.hh"
#include "graphcore/Graph.hh"

namespace ehunter
{

class SoftclippingAligner
{
public:
    SoftclippingAligner(
        const graphtools::Graph* graphPtr, const std::string& alignerName, int kmerLenForAlignment, int paddingLength,
        int seedAffixTrimLength);
    std::list<graphtools::GraphAlignment> align(const std::string& query) const;

private:
    graphtools::GappedGraphAligner aligner_;
};

}
