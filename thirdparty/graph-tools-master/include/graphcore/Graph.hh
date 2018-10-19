// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Copyright (c) 2018 Illumina, Inc.
// All rights reserved.

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

#include <algorithm>
#include <cstdint>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "graphutils/PairHashing.hh"

namespace graphtools
{

using NodeId = uint32_t;
using NodeIdPair = std::pair<NodeId, NodeId>;
using SortedLabels = std::set<std::string>;
using Labels = std::unordered_set<std::string>;
using AdjacencyList = std::vector<std::set<NodeId>>;

struct Node
{
    std::string name;
    std::string sequence;
    std::vector<std::string> sequence_expansion;
};

/**
 * Sequence graph that can hold degenerate nucleotide sequences
 */
class Graph
{
public:
    explicit Graph(size_t num_nodes = 0, std::string const& id = "", bool is_sequence_expansion_required = true)
        : graphId(id)
    {
        init(num_nodes);
        is_sequence_expansion_required_ = is_sequence_expansion_required;
    }

    virtual ~Graph() = default;

    size_t numNodes() const { return nodes_.size(); }
    size_t numEdges() const { return edge_labels_.size(); }
    bool isSequenceExpansionRequired() const { return is_sequence_expansion_required_; }
    const std::string& nodeName(NodeId node_id) const;
    void setNodeName(NodeId node_id, const std::string& node_name);
    const std::string& nodeSeq(NodeId node_id) const;
    void setNodeSeq(NodeId node_id, const std::string& sequence);
    const std::vector<std::string>& nodeSeqExpansion(NodeId node_id) const;
    void addEdge(NodeId source_node_id, NodeId sink_node_id);
    bool hasEdge(NodeId source_node_id, NodeId sink_node_id) const;

    SortedLabels allLabels() const;
    const Labels& edgeLabels(NodeId source_node_id, NodeId sink_node_id) const;
    // All edges in the graph with this label
    std::set<NodeIdPair> edgesWithLabel(const std::string& label) const;
    /**
     * All label to an existing edge
     * @throws if source -> sink is not an edge in the graph
     */
    void addLabelToEdge(NodeId source_node_id, NodeId sink_node_id, const std::string& label);
    // Remove this label from all edges
    void eraseLabel(const std::string& label);

    // this cannot be const if graphs are to be assigned. Currently this happens in unit tests.
    std::string graphId;
    const std::set<NodeId>& successors(NodeId node_id) const;
    const std::set<NodeId>& predecessors(NodeId node_id) const;

private:
    void init(size_t num_nodes);
    void assertNodeExists(NodeId node_id) const;
    void assertEdgeExists(NodeIdPair edge) const;
    std::vector<Node> nodes_;
    bool is_sequence_expansion_required_;
    std::unordered_map<NodeIdPair, Labels> edge_labels_;
    AdjacencyList adjacency_list_;
    AdjacencyList reverse_adjacency_list_;
};

class ReverseGraph
{
    const Graph& graph_;
public:
    explicit ReverseGraph(const Graph& graph)
        : graph_(graph)
    {
    }

    size_t numNodes() const { return graph_.numNodes(); }
    size_t numEdges() const { return graph_.numEdges(); }
    bool isSequenceExpansionRequired() const { return graph_.isSequenceExpansionRequired(); }
    const std::string& nodeName(NodeId nodeId) const { return graph_.nodeName(nodeId); }
    const std::string nodeSeq(NodeId nodeId) const
    {
        std::string ret = graph_.nodeSeq(nodeId);
        std::reverse(ret.begin(), ret.end());
        return ret;
    }
    bool hasEdge(NodeId sourceNodeId, NodeId sinkNodeId) const { return graph_.hasEdge(sourceNodeId, sinkNodeId); }

    SortedLabels allLabels() const { return graph_.allLabels(); }
    const Labels& edgeLabels(NodeId sourceNodeId, NodeId sinkNodeId) const
    {
        return graph_.edgeLabels(sourceNodeId, sinkNodeId);
    }

    // All edges in the graph with this label
    std::set<NodeIdPair> edgesWithLabel(const std::string& label) const { return graph_.edgesWithLabel(label); }

    const std::set<NodeId>& successors(NodeId nodeId) const { return graph_.predecessors(nodeId); }
    const std::set<NodeId>& predecessors(NodeId nodeId) const { return graph_.successors(nodeId); }
};

}