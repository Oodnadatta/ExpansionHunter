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

#include "genotyping/ShortRepeatGenotyper.hh"

#include <array>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "common/Common.hh"
#include "genotyping/RepeatGenotype.hh"

using std::array;
using std::cerr;
using std::endl;
using std::map;
using std::string;
using std::vector;

using namespace ehunter;

TEST(CalculateMoleculeProportions, TypicalHaplotype_ProportionsCalculated)
{
    const int32_t allele_size_in_units = 2;
    const int32_t maxRepeatSizeInUnits = 25;
    const double propCorrectMolecules = 0.97;
    QuantifierOfMoleculesGeneratedByAllele alleleQuantifier(
        allele_size_in_units, maxRepeatSizeInUnits, propCorrectMolecules);

    EXPECT_DOUBLE_EQ(2.2885056508333023e-08, alleleQuantifier.propMoleculesOfGivenSize(25));
    EXPECT_DOUBLE_EQ(0.97087262363952287, alleleQuantifier.propMoleculesShorterThan(3));
    EXPECT_DOUBLE_EQ(0.029127376360477131, alleleQuantifier.propMoleculesAtLeast(3));
}

TEST(CalcFlankingLoglik, TypicalFlankingReads_LoglikelihoodsCalculated)
{
    const int32_t maxRepeatSizeInUnits = 25;
    const double propCorrectMolecules = 0.97;
    const vector<int32_t> allele_sizes_in_units = { 2, 3 };
    ShortRepeatGenotypeLikelihoodEstimator likelihoodEstimator(
        maxRepeatSizeInUnits, propCorrectMolecules, allele_sizes_in_units);
    EXPECT_DOUBLE_EQ(-0.015100313643051028, likelihoodEstimator.CalcFlankingLoglik(2));
    EXPECT_DOUBLE_EQ(-17.592794352808042, likelihoodEstimator.CalcFlankingLoglik(25));
}

TEST(CalcSpanningLoglik, TypicalSpanningReads_LoglikelihoodsCalcualted)
{
    const int32_t maxRepeatSizeInUnits = 25;
    const double propCorrectMolecules = 0.97;
    const vector<int32_t> genotype = { 2, 3 };
    ShortRepeatGenotypeLikelihoodEstimator likelihoodEstimator(maxRepeatSizeInUnits, propCorrectMolecules, genotype);
    EXPECT_DOUBLE_EQ(-0.7236052500150770, likelihoodEstimator.CalcSpanningLoglik(3));
    EXPECT_DOUBLE_EQ(-4.2301631473350575, likelihoodEstimator.CalcSpanningLoglik(4));
}

TEST(CalcGenotypeLoglik, ShortGenotypes_LoglikelihoodsCalculated)
{
    const int32_t maxRepeatSizeInUnits = 25;
    const double propCorrectMolecules = 0.97;

    const map<int32_t, int32_t> flankingSizesAndCounts = { { 1, 2 }, { 2, 3 }, { 10, 1 } };
    const map<int32_t, int32_t> spanningSizesAndCounts = { { 3, 4 }, { 5, 5 } };
    const CountTable countsOfFlankingReads(flankingSizesAndCounts);
    const CountTable countsOfSpanningReads(spanningSizesAndCounts);

    {
        const vector<int32_t> genotype = { 3, 5 };
        const ShortRepeatGenotypeLikelihoodEstimator likelihoodEstimator(
            maxRepeatSizeInUnits, propCorrectMolecules, genotype);
        const double log_likelihood = likelihoodEstimator.CalcLogLik(countsOfFlankingReads, countsOfSpanningReads);

        const double expectedLogLikelihood = -21.591945631259129;
        EXPECT_DOUBLE_EQ(expectedLogLikelihood, log_likelihood);
    }

    {
        const vector<int32_t> genotype = { 3, 10 };
        const ShortRepeatGenotypeLikelihoodEstimator likelihoodEstimator(
            maxRepeatSizeInUnits, propCorrectMolecules, genotype);
        const double log_likelihood = likelihoodEstimator.CalcLogLik(countsOfFlankingReads, countsOfSpanningReads);

        const double expectedLogLikelihood = -42.567968025644028;
        EXPECT_DOUBLE_EQ(expectedLogLikelihood, log_likelihood);
    }

    {

        const vector<int32_t> genotype = { 10, 10 };
        const ShortRepeatGenotypeLikelihoodEstimator likelihoodEstimator(
            maxRepeatSizeInUnits, propCorrectMolecules, genotype);
        const double log_likelihood = likelihoodEstimator.CalcLogLik(countsOfFlankingReads, countsOfSpanningReads);

        const double expectedLogLikelihood = -158.36482963578563;
        EXPECT_DOUBLE_EQ(expectedLogLikelihood, log_likelihood);
    }
}

TEST(CalcDiploidGenotypeLoglik, TypicalGenotypeLoglikelihoods_Calculated)
{
    const int32_t maxRepeatSizeInUnits = 25;
    const double propCorrectMolecules = 0.97;

    const map<int32_t, int32_t> flankingSizesAndCounts = { { 1, 2 }, { 2, 3 }, { 25, 10 } };
    const map<int32_t, int32_t> spanningSizesAndCounts = { { 5, 5 } };
    const CountTable countsOfFlankingReads(flankingSizesAndCounts);
    const CountTable countsOfSpanningReads(spanningSizesAndCounts);

    const vector<int32_t> genotype = { 5, 25 };
    const ShortRepeatGenotypeLikelihoodEstimator likelihoodEstimator(
        maxRepeatSizeInUnits, propCorrectMolecules, genotype);
    const double log_likelihood = likelihoodEstimator.CalcLogLik(countsOfFlankingReads, countsOfSpanningReads);

    const double expectedLogLikelihood = -7.3838630069778066;
    EXPECT_DOUBLE_EQ(expectedLogLikelihood, log_likelihood);
}

TEST(RepeatGenotyping, TypicalDiploidRepeat_Genotyped)
{
    const int32_t repeatUnitLen = 6;
    const int32_t maxRepeatSizeInUnits = 25;
    const double propCorrectMolecules = 0.97;

    const map<int32_t, int32_t> flankingSizesAndCounts = { { 1, 2 }, { 2, 3 }, { 10, 1 } };
    const map<int32_t, int32_t> spanningSizesAndCounts = { { 3, 4 }, { 5, 5 } };
    const CountTable countsOfFlankingReads(flankingSizesAndCounts);
    const CountTable countsOfSpanningReads(spanningSizesAndCounts);

    vector<int32_t> candidateAlleleSizes;
    for (int32_t candidateAlleleSize = 0; candidateAlleleSize != 26; ++candidateAlleleSize)
    {
        candidateAlleleSizes.push_back(candidateAlleleSize);
    }

    const ShortRepeatGenotyper shortRepeatGenotyper(repeatUnitLen, maxRepeatSizeInUnits, propCorrectMolecules);

    const RepeatGenotype genotype = shortRepeatGenotyper.genotypeRepeatWithTwoAlleles(
        countsOfFlankingReads, countsOfSpanningReads, candidateAlleleSizes);

    const RepeatGenotype expectedGenotype(6, { 3, 5 });
    EXPECT_EQ(expectedGenotype, genotype);
}

TEST(RepeatGenotyping, TypicalHaploidRepeat_Genotyped)
{
    const int32_t repeatUnitLen = 6;
    const int32_t maxRepeatSizeInUnits = 25;
    const double propCorrectMolecules = 0.97;

    const map<int32_t, int32_t> flankingSizesAndCounts = { { 1, 2 }, { 2, 3 }, { 10, 1 } };
    const map<int32_t, int32_t> spanningSizesAndCounts = { { 3, 4 }, { 5, 5 } };
    const CountTable countsOfFlankingReads(flankingSizesAndCounts);
    const CountTable countsOfSpanningReads(spanningSizesAndCounts);

    vector<int32_t> candidateAlleleSizes;
    for (int32_t candidateAlleleSize = 0; candidateAlleleSize != 26; ++candidateAlleleSize)
    {
        candidateAlleleSizes.push_back(candidateAlleleSize);
    }

    const ShortRepeatGenotyper shortRepeatGenotyper(repeatUnitLen, maxRepeatSizeInUnits, propCorrectMolecules);

    const RepeatGenotype genotype = shortRepeatGenotyper.genotypeRepeatWithOneAllele(
        countsOfFlankingReads, countsOfSpanningReads, candidateAlleleSizes);

    const RepeatGenotype expectedGenotype(repeatUnitLen, { 5 });
    EXPECT_EQ(expectedGenotype, genotype);
}

TEST(RepeatGenotyping, ExpandedRepeatWithOneAllele_Genotyped)
{
    const int32_t repeatUnitLen = 6;
    const int32_t maxRepeatSizeInUnits = 25;
    const double propCorrectMolecules = 0.97;

    const map<int32_t, int32_t> flankingSizesAndCounts = { { 1, 2 }, { 2, 3 }, { 10, 1 }, { 25, 8 } };
    const map<int32_t, int32_t> spanningSizesAndCounts = { { 3, 1 }, { 5, 1 } };
    const CountTable countsOfFlankingReads(flankingSizesAndCounts);
    const CountTable countsOfSpanningReads(spanningSizesAndCounts);

    vector<int32_t> candidateAlleleSizes;
    for (int32_t candidateAlleleSize = 0; candidateAlleleSize != 26; ++candidateAlleleSize)
    {
        candidateAlleleSizes.push_back(candidateAlleleSize);
    }

    const ShortRepeatGenotyper shortRepeatGenotyper(repeatUnitLen, maxRepeatSizeInUnits, propCorrectMolecules);

    const RepeatGenotype genotype = shortRepeatGenotyper.genotypeRepeatWithOneAllele(
        countsOfFlankingReads, countsOfSpanningReads, candidateAlleleSizes);

    const RepeatGenotype expectedGenotype(repeatUnitLen, { 25 });

    EXPECT_EQ(expectedGenotype, genotype);
}
