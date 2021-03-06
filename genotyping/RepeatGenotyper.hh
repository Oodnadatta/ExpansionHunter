//
// Expansion Hunter
// Copyright (c) 2016 Illumina, Inc.
//
// Author: Egor Dolzhenko <edolzhenko@illumina.com>,
//         Mitch Bekritsky <mbekritsky@illumina.com>, Richard Shaw
// Concept: Michael Eberle <meberle@illumina.com>
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

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include "common/Common.hh"
#include "common/CountTable.hh"
#include "common/Parameters.hh"
#include "genotyping/RepeatGenotype.hh"
#include "region_spec/LocusSpecification.hh"

namespace ehunter
{

class RepeatGenotyper
{
public:
    RepeatGenotyper(
        double haplotypeDepth, AlleleCount expectedAlleleCount, int32_t repeatUnitLen, int32_t maxNumUnitsInRead,
        double propCorrectMolecules, const CountTable& countsOfSpanningReads, const CountTable& countsOfFlankingReads,
        const CountTable& countsOfRepeatReads)
        : expectedAlleleCount_(expectedAlleleCount)
        , repeatUnitLen_(repeatUnitLen)
        , maxNumUnitsInRead_(maxNumUnitsInRead)
        , haplotypeDepth_(haplotypeDepth)
        , propCorrectMolecules_(propCorrectMolecules)
        , countsOfSpanningReads_(countsOfSpanningReads)
        , countsOfFlankingReads_(countsOfFlankingReads)
        , countsOfInrepeatReads_(countsOfRepeatReads)
    {
    }

    boost::optional<RepeatGenotype> genotypeRepeat(const std::vector<int32_t>& alleleSizeCandidates) const;

private:
    void extendGenotypeWhenBothAllelesAreRepeat(RepeatGenotype& genotype, int numRepeatReads) const;
    void extendGenotypeWhenOneAlleleIsRepeat(RepeatGenotype& genotype, int numRepeatReads) const;
    void extendGenotypeWhenBothAllelesAreFlanking(RepeatGenotype& genotype) const;
    void extendGenotypeWhenOneAlleleIsFlanking(RepeatGenotype& genotype) const;

    void estimateFlankingAlleleSize(
        int32_t& flankingAlleleSize, int32_t& flankingAlleleCiLower, int32_t& flankingAlleleCiUpper) const;
    void estimateRepeatAlleleSize(
        int32_t numIrrs, int32_t& flankingAlleleSize, int32_t& flankingAlleleCiLower,
        int32_t& flankingAlleleCiUpper) const;

    int countFullLengthRepeatReads() const;

    const AlleleCount expectedAlleleCount_;
    const int32_t repeatUnitLen_;
    const int32_t maxNumUnitsInRead_;
    const double haplotypeDepth_;
    const double propCorrectMolecules_;
    const CountTable countsOfSpanningReads_;
    const CountTable countsOfFlankingReads_;
    const CountTable countsOfInrepeatReads_;
};

int countFullLengthRepeatReads(
    int maxNumUnitsInRead, const CountTable& countsOfFlankingReads, const CountTable& countsOfInrepeatReads);

}
