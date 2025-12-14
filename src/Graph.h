#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
 
 using namespace std;

struct Node {
    string id;
    string type;
    string name;
    unordered_map<string, string> properties;
};

struct Edge {
    string sourceId;
    string targetId;
    string type;
};

class Graph {

private:
    unordered_map<string, Node> nodes;
    unordered_map<string, vector<Edge>> adjacencyList;
    unordered_map<string, vector<Edge>> reverseAdjacencyList;
    
    string findNodeIdByName(const string& name);

public:
    void addNode(const string& id, const string& type, const string& name, const unordered_map<string, string>& properties = {});

    void addEdge(const string& sourceId, const string& targetId, const string& type);
    
    // Returns a list of strings describing the path, or empty if no path found
    vector<string> shortestPath(const string& sourceName, const string& targetName);

    // Returns a list of strings describing reachable nodes (DFS)
    vector<string> getReachableNodes(const string& sourceName);

    // Returns a list of strings describing nodes that can control the target (Reverse BFS/DFS)
    vector<string> getControllers(const string& targetName);

    // Returns a list of edges forming the shortest path to a high-value target
    vector<Edge> findPathToHighValue(const string& sourceName);

    const unordered_map<string, Node>& getNodes() const { return nodes; }
    const unordered_map<string, vector<Edge>>& getReverseAdjacencyList() const { return reverseAdjacencyList; }


};
