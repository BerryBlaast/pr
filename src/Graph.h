#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

struct Node {
    std::string id;
    std::string type;
    std::string name;
    std::unordered_map<std::string, std::string> properties;
};

struct Edge {
    std::string sourceId;
    std::string targetId;
    std::string type;
};

class Graph {
public:
    void addNode(const std::string& id, const std::string& type, const std::string& name, const std::unordered_map<std::string, std::string>& properties = {});
    void addEdge(const std::string& sourceId, const std::string& targetId, const std::string& type);
    
    // Returns a list of strings describing the path, or empty if no path found
    std::vector<std::string> shortestPath(const std::string& sourceName, const std::string& targetName);

    // Returns a list of strings describing reachable nodes (DFS)
    std::vector<std::string> getReachableNodes(const std::string& sourceName);

    // Returns a list of strings describing nodes that can control the target (Reverse BFS/DFS)
    std::vector<std::string> getControllers(const std::string& targetName);

    // Returns a list of edges forming the shortest path to a high-value target
    std::vector<Edge> findPathToHighValue(const std::string& sourceName);

    const std::unordered_map<std::string, Node>& getNodes() const { return nodes; }
    const std::unordered_map<std::string, std::vector<Edge>>& getReverseAdjacencyList() const { return reverseAdjacencyList; }

private:
    std::unordered_map<std::string, Node> nodes;
    std::unordered_map<std::string, std::vector<Edge>> adjacencyList;
    std::unordered_map<std::string, std::vector<Edge>> reverseAdjacencyList;
    
    std::string findNodeIdByName(const std::string& name);
};
