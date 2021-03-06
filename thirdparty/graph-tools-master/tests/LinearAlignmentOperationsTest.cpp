//
// GraphTools library
// Copyright (c) 2018 Illumina, Inc.
// All rights reserved.
//
// Author: Egor Dolzhenko <edolzhenko@illumina.com>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.

// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "graphalign/LinearAlignmentOperations.hh"

#include "gtest/gtest.h"

#include <stdexcept>

using namespace graphtools;

using std::list;
using std::make_pair;
using std::string;

TEST(CheckingConsistencyOfAlignments, ConsistentAlignment_CheckPassed)
{
    //   ref: CCCTTCCTTAATT---T----------
    //           ||| ||  ||   |
    // query: ---TTCGNN--TTGGGTCCCCCCCCCC
    string reference = "CCCTTCCTTAATTT";
    string query = "TTCGNNTTGGGTCCCCCCCCCC";

    Alignment alignment(3, "3M1X2N2D2M3I1M10S");

    EXPECT_TRUE(checkConsistency(alignment, reference, query));
}

TEST(CheckingConsistencyOfAlignments, InconsistentAlignment_CheckFailed)
{
    const string query = "AAAT";
    const string reference = "AAAG";

    EXPECT_FALSE(checkConsistency(Alignment(0, "4M"), reference, query));
    EXPECT_FALSE(checkConsistency(Alignment(0, "3M1X2S"), reference, query));
    EXPECT_FALSE(checkConsistency(Alignment(0, "3M2X"), reference, query));
    EXPECT_FALSE(checkConsistency(Alignment(0, "1M"), reference, query));
}

TEST(GettingSequencesForEachOperation, TypicalAlignment_SequencePairs)
{
    //   ref: CCCTTCCTTAATT---T----------
    //           ||| ||  ||   |
    // query: ---TTCGNN--TTGGGTCCCCCCCCCC
    string reference = "CCCTTCCTTAATTT";
    string query = "TTCGNNTTGGGTCCCCCCCCCC";

    Alignment alignment(3, "3M1X2N2D2M3I1M10S");

    list<StringPair> expected_sequence_pieces
        = { { "TTC", "TTC" }, { "C", "G" },  { "TT", "NN" }, { "AA", "" },
            { "TT", "TT" },   { "", "GGG" }, { "T", "T" },   { "", "CCCCCCCCCC" } };

    ASSERT_EQ(expected_sequence_pieces, getSequencesForEachOperation(alignment, reference, query));
}

TEST(CheckingIfAlignmentsAreBookended, AdjacentAlignments_AreBookended)
{
    // CCCG--TAG
    //   ||  |||
    // ATCGATTAG
    Alignment first_alignment(2, "2S2M");
    Alignment second_alignment(4, "2I3M");

    EXPECT_TRUE(checkIfBookended(first_alignment, second_alignment));
}

TEST(CheckingIfAlignmentsAreBookended, NonAdjacentAlignments_NotBookended)
{
    // CCCG--TAG
    //   ||   ||
    // ATCGATTAG
    Alignment first_alignment(2, "2S2M");
    Alignment second_alignment(5, "2M");

    EXPECT_FALSE(checkIfBookended(first_alignment, second_alignment));
}

TEST(CheckingIfAlignmentsAreBookended, FirstAlignmentEndsInSoftclip_NotBookended)
{
    Alignment first_alignment(0, "2M2S");
    Alignment second_alignment(2, "2M");

    EXPECT_FALSE(checkIfBookended(first_alignment, second_alignment));
}

TEST(CheckingIfAlignmentsAreBookended, SecondAlignmentStartsInSoftclip_NotBookended)
{
    Alignment first_alignment(0, "2M");
    Alignment second_alignment(2, "2S2M");

    EXPECT_FALSE(checkIfBookended(first_alignment, second_alignment));
}

TEST(CheckingIfAlignmentsAreBookended, OneOfTheAlignmentsEntirelySoftclipped_AreBookended)
{
    {
        Alignment first_alignment(0, "2M");
        Alignment second_alignment(2, "4S");

        EXPECT_TRUE(checkIfBookended(first_alignment, second_alignment));
    }

    {
        Alignment first_alignment(0, "2M3S");
        Alignment second_alignment(2, "4S");

        EXPECT_TRUE(checkIfBookended(first_alignment, second_alignment));
    }

    {
        Alignment first_alignment(0, "2S");
        Alignment second_alignment(0, "2M");

        EXPECT_TRUE(checkIfBookended(first_alignment, second_alignment));
    }

    {
        Alignment first_alignment(0, "2S");
        Alignment second_alignment(0, "1S2M");

        EXPECT_TRUE(checkIfBookended(first_alignment, second_alignment));
    }
}

TEST(MergingAlignments, NotBookendededAlignments_ExceptionThrown)
{
    Alignment first_alignment(0, "2M");
    Alignment second_alignment(3, "1M");
    EXPECT_ANY_THROW(mergeAlignments(first_alignment, second_alignment));
}

TEST(MergingAlignments, AlignmentsWithDifferentBoundaryOperations_Merged)
{
    // CCCG--TAG
    //   ||  |||
    // ATCGATTAG
    Alignment first_alignment(2, "2S2M");
    Alignment second_alignment(4, "2I3M");

    Alignment merged_alignment = mergeAlignments(first_alignment, second_alignment);

    Alignment expected_alignment(2, "2S2M2I3M");
    EXPECT_EQ(expected_alignment, merged_alignment);
}

TEST(MergingAlignments, AlignmentsWithSameBoundaryOperation_Merged)
{
    {
        // CCCG---TAG
        //   ||   |||
        // ATCGATGTAG
        Alignment first_alignment(2, "2S2M1I");
        Alignment second_alignment(4, "2I3M");

        Alignment merged_alignment = mergeAlignments(first_alignment, second_alignment);

        Alignment expected_alignment(2, "2S2M3I3M");
        EXPECT_EQ(expected_alignment, merged_alignment);
    }

    {
        Alignment first_alignment(0, "2M3S");
        Alignment second_alignment(2, "4S");

        Alignment merged_alignment = mergeAlignments(first_alignment, second_alignment);

        Alignment expected_alignment(0, "2M7S");
        EXPECT_EQ(expected_alignment, merged_alignment);
    }

    {

        Alignment first_alignment(0, "2S");
        Alignment second_alignment(0, "1S2M");

        Alignment merged_alignment = mergeAlignments(first_alignment, second_alignment);

        Alignment expected_alignment(0, "3S2M");
        EXPECT_EQ(expected_alignment, merged_alignment);
    }
}

TEST(ScoringAlignment, TypicalAlignment_Scored)
{
    Alignment alignment(3, "2S3M1X2N2D2M3I1M10S");
    const int32_t match_score = 1;
    const int32_t mismatch_score = -2;
    const int32_t gap_score = -3;

    //  2S    3M       1X    2N       2D    2M       3I    1M    10S
    // 2*0 + 3*1 + 1*(-2) + 2*0 + 2*(-3) + 2*1 + 3*(-3) + 1*1 + 10*0 = -11
    const int32_t score = scoreAlignment(alignment, match_score, mismatch_score, gap_score);

    EXPECT_EQ(-11, score);
}

TEST(PrettyPrintingAlignments, TypicalAlignment_PrettyPrinted)
{
    //   ref: CCCTTCCTTAATT---T----------
    //           |||     ||   |
    // query: ---TTCGNN--TTGGGTCCCCCCCCCC
    string reference = "CCCTTCCTTAATTT";
    string query = "TTCGNNTTGGGTCCCCCCCCCC";

    Alignment alignment(3, "3M1X2N2D2M3I1M10S");

    const string alignment_encoding = prettyPrint(alignment, reference, query);

    const string expected_encoding = "TTCCTTAATT---T----------\n"
                                     "|||     ||   |          \n"
                                     "TTCGNN--TTGGGTCCCCCCCCCC";

    ASSERT_EQ(expected_encoding, alignment_encoding);
}
