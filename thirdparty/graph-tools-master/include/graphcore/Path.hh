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

#pragma once

#include <cstdint>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "graphcore/Graph.hh"

namespace graphtools
{

/**
 * A path in a sequence graph is given by (1) a sequence of nodes and (2) start/end position on the first/last node. The
 * start/end positions are 0-based and form a half-open interval.
 */
class Path
{
public:
    typedef std::vector<NodeId>::const_iterator const_iterator;
    // The constructor checks if the inputs define a well-formed path.
    Path(const Graph* graph_raw_ptr, int32_t start_position, const std::vector<NodeId>& nodes, int32_t end_position);
    ~Path();
    Path(const Path& other);
    Path(Path&& other) noexcept;
    Path& operator=(const Path& other);
    Path& operator=(Path&& other) noexcept;
    bool operator==(const Path& other) const;
    bool operator<(const Path& path) const;

    const_iterator begin() const;
    const_iterator end() const;

    // Ids of nodes overlapped by the path
    std::vector<NodeId> const& nodeIds() const;
    size_t numNodes() const;
    // Sequence of the entire path
    std::string seq() const;
    // Piece of node sequence that the path overlaps
    std::string getNodeSeq(size_t node_index) const;
    const Graph* graphRawPtr() const;
    std::string encode() const;
    int32_t startPosition() const;
    int32_t endPosition() const;
    size_t length() const;
    NodeId getNodeIdByIndex(size_t node_index) const;
    int32_t getStartPositionOnNodeByIndex(size_t node_index) const;
    int32_t getEndPositionOnNodeByIndex(size_t node_index) const;
    size_t getNodeOverlapLengthByIndex(size_t node_index) const;

    bool checkOverlapWithNode(NodeId node_id) const;
    int32_t getDistanceFromPathStart(NodeId node, int32_t offset) const;

    // Path modifiers
    // Moves start position by a specified length; the path gets longer/shorter if shift_len is positive/negative
    // respectively.
    void shiftStartAlongNode(int32_t shift_len);
    // Moves end position by a specified length; the path gets longer/shorter if shift_len is positive/negative
    // respectively.
    void shiftEndAlongNode(int32_t shift_len);
    // Moves path start to the end of the specified node. The new node must be a predecessor of the previous start node.
    void extendStartToNode(NodeId node_id);
    // Moves path start to the start of the specified node. The new node must be a predecessor of the previous start
    // node.
    void extendStartToIncludeNode(NodeId node_id);
    // Moves path end to the start of the specified node. The new node must be a successor of the previous start node.
    void extendEndToNode(NodeId node_id);
    // Moves path end to the end of the specified node. The new node must be a successor of the previous start node.
    void extendEndToIncludeNode(NodeId node_id);
    // Moves path start to the start of the next node in the path.
    void removeStartNode();
    // Moves path end to the end of the previous node in the path.
    void removeEndNode();
    // Moves path start to the start of the next node if the start has zero-length overlap with the corresponding node;
    // does nothing if path spans only one node.
    void removeZeroLengthStart();
    // Moves path end to the end of the end of the previous node if the end of the path has zero-length overlap with the
    // corresponding node; does nothing if path spans only one node.
    void removeZeroLengthEnd();
    // Shortens the start of the path by a specified length.
    void shrinkStartBy(int32_t shrink_len);
    // Shortens the end of the path by a specified length.
    void shrinkEndBy(int32_t shrink_len);
    // Shortens the path by the specified lengths from each direction.
    void shrinkBy(int32_t start_shrink_len, int32_t end_shrink_len);

    NodeId firstNodeId() const { return nodeIds().front(); }
    NodeId lastNodeId() const { return nodeIds().back(); }

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

std::ostream& operator<<(std::ostream& os, const Path& path);

class ReversePath
{
    Path& path_;

public:
    explicit ReversePath(Path& path)
        : path_(path)
    {
    }

    // TODO: add methods as needed

    NodeId firstNodeId() const { return path_.lastNodeId(); }
    NodeId lastNodeId() const { return path_.firstNodeId(); }

    int32_t startPosition() const
    {
        return path_.graphRawPtr()->nodeSeq(path_.lastNodeId()).length() - path_.endPosition();
    }

    int32_t endPosition() const
    {
        return path_.graphRawPtr()->nodeSeq(path_.firstNodeId()).length() - path_.startPosition();
    }

    std::string seq() const
    {
        std::string ret = path_.seq();
        std::reverse(ret.begin(), ret.end());
        return ret;
    }

    void shiftEndAlongNode(int32_t shift_len) { path_.shiftStartAlongNode(shift_len); }
    void extendEndToNode(NodeId node_id) { path_.extendStartToNode(node_id); }

    friend std::ostream& operator<<(std::ostream& os, const ReversePath& path)
    {
        return os << "reverse path of: " << path.path_;
    }
};

class ConstReversePath
{
    const Path& path_;

public:
    explicit ConstReversePath(const Path& path)
        : path_(path)
    {
    }

    // TODO: add methods as needed

    NodeId firstNodeId() const { return path_.lastNodeId(); }
    NodeId lastNodeId() const { return path_.firstNodeId(); }

    int32_t startPosition() const
    {
        return path_.graphRawPtr()->nodeSeq(path_.lastNodeId()).length() - path_.endPosition();
    }

    int32_t endPosition() const
    {
        return path_.graphRawPtr()->nodeSeq(path_.firstNodeId()).length() - path_.startPosition();
    }

    const Graph* graphRawPtr() const { return path_.graphRawPtr(); }
};

}
