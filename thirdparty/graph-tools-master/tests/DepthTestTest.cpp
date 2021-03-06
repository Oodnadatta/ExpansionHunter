//
// GraphTools library
// Copyright (c) 2018 Illumina, Inc.
// All rights reserved.
//
// Author: Sai Chen <schen6@illumina.com>,
//         Egor Dolzhenko <edolzhenko@illumina.com>
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

#include "graphutils/DepthTest.hh"

#include "gtest/gtest.h"

TEST(TestingCoverageDepth, ReasonableCoverage_TestPasses)
{
    DepthTest depth_test(20, 5, 0.05, 0.001);
    EXPECT_TRUE(depth_test.testReadCount(15));
    EXPECT_TRUE(depth_test.testReadCount(30));
}

TEST(TestingCoverageDepth, PoorCoverage_TestFails)
{
    DepthTest depth_test(20, 5, 0.05, 0.001);
    EXPECT_FALSE(depth_test.testReadCount(10));
    EXPECT_FALSE(depth_test.testReadCount(40));
}
