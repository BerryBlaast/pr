#include "Graph.h"
#include <queue>
#include <algorithm>
#include <unordered_set>
using namespace std;

void Graph::addNode(const string& id, const string& type, const string& name, const unordered_map<string, string>& properties) {
    nodes[id] = {id, type, name, properties};
}

void Graph::addEdge(const string& sourceId, const string& targetId, const string& type) {
    adjacencyList[sourceId].push_back({sourceId, targetId, type});
    reverseAdjacencyList[targetId].push_back({sourceId, targetId, type});
}

string Graph::findNodeIdByName(const string& name) {
    for (const auto& pair : nodes) {
        if (pair.second.name == name) {
            return pair.first;
        }
    }
    return "";
}

vector<string> Graph::shortestPath(const string& sourceName, const string& targetName) {
    string startId = findNodeIdByName(sourceName);
    string endId = findNodeIdByName(targetName);

    if (startId.empty()) {
        cerr << "Source node not found: " << sourceName << endl;
        return {};
    }
    if (endId.empty()) {
        cerr << "Target node not found: " << targetName << endl;
        return {};
    }

    queue<string> q;
    q.push(startId);

    unordered_map<string, string> parent; // To reconstruct path: child -> parent
    unordered_map<string, string> edgeType; // To store edge type: child -> type from parent
    unordered_set<string> visited;

    visited.insert(startId);

    bool found = false;

    while (!q.empty()) {
        string currentId = q.front();
        q.pop();

        if (currentId == endId) {
            found = true;
            break;
        }

        if (adjacencyList.find(currentId) != adjacencyList.end()) {
            for (const auto& edge : adjacencyList[currentId]) {
                if (visited.find(edge.targetId) == visited.end()) {
                    visited.insert(edge.targetId);
                    parent[edge.targetId] = currentId;
                    edgeType[edge.targetId] = edge.type;
                    q.push(edge.targetId);
                }
            }
        }
    }

    if (found) {
        vector<string> path;
        string curr = endId;
        while (curr != startId) {
            string p = parent[curr];
            string type = edgeType[curr];
            string nodeName = nodes[curr].name;
            string nodeType = nodes[curr].type;
            
            path.push_back("(" + type + ") -> " + nodeName + " [" + nodeType + "]");
            curr = p;
        }
        path.push_back(nodes[startId].name + " [" + nodes[startId].type + "]");
        reverse(path.begin(), path.end());
        return path;
    }

    return {};
}
vector<string> Graph::getReachableNodes(const string& sourceName) {
    string startId = findNodeIdByName(sourceName);
    if (startId.empty()) {
        cerr << "Source node not found: " << sourceName << endl;
        return {};
    }

    vector<string> result;
    unordered_set<string> visited;
    queue<string> q;

    q.push(startId);
    visited.insert(startId);

    while (!q.empty()) {
        string currentId = q.front();
        q.pop();

        if (currentId != startId) {
             result.push_back(nodes[currentId].name + " [" + nodes[currentId].type + "]");
        }

        if (adjacencyList.find(currentId) != adjacencyList.end()) {
            for (const auto& edge : adjacencyList[currentId]) {
                if (visited.find(edge.targetId) == visited.end()) {
                    visited.insert(edge.targetId);
                    q.push(edge.targetId);
                }
            }
        }
    }
    return result;
}

vector<string> Graph::getControllers(const string& targetName) {
    string targetId = findNodeIdByName(targetName);
    if (targetId.empty()) {
        cerr << "Target node not found: " << targetName << endl;
        return {};
    }

    vector<string> result;
    unordered_set<string> visited;
    queue<string> q;

    q.push(targetId);
    visited.insert(targetId);

    while (!q.empty()) {
        string currentId = q.front();
        q.pop();

        if (currentId != targetId) {
             result.push_back(nodes[currentId].name + " [" + nodes[currentId].type + "]");
        }

        if (reverseAdjacencyList.find(currentId) != reverseAdjacencyList.end()) {
            for (const auto& edge : reverseAdjacencyList[currentId]) {
                if (visited.find(edge.sourceId) == visited.end()) {
                    visited.insert(edge.sourceId);
                    q.push(edge.sourceId);
                }
            }
        }
    }
    return result;
}

bool isHighValueTarget(const Node& node) {
    // Groups
    if (node.type == "Group") {
        if (node.name.find("DOMAIN ADMINS") != string::npos ||
            node.name.find("ENTERPRISE ADMINS") != string::npos ||
            node.name.find("SCHEMA ADMINS") != string::npos) {
            return true;
        }
    }
    // Users
    if (node.type == "User") {
        if (node.name.find("ADMINISTRATOR@") == 0) { // Starts with
            return true;
        }
    }
    // Computers (Domain Controllers)
    if (node.type == "Computer") {
        // Check properties if available
        if (node.properties.count("distinguishedname")) {
            if (node.properties.at("distinguishedname").find("OU=Domain Controllers") != string::npos) {
                return true;
            }
        }
        // Fallback to naming convention heuristic
        if (node.name.find("DC") != string::npos) { // Simple heuristic
            return true;
        }
    }
    return false;
}

vector<Edge> Graph::findPathToHighValue(const string& sourceName) {
    string startId = findNodeIdByName(sourceName);
    if (startId.empty()) {
        cerr << "Source node not found: " << sourceName << endl;
        return {};
    }

    queue<string> q;
    q.push(startId);

    unordered_map<string, Edge> parentEdge; // childId -> Edge from parent
    unordered_set<string> visited;
    visited.insert(startId);

    string targetId = "";

    while (!q.empty()) {
        string currentId = q.front();
        q.pop();

        // Check if current node is high value (but not the start node itself, unless it's a path to itself? No, usually we want escalation)
        // Actually, if we start at a high value node, the path is size 0 or we can say we are already there.
        // But the word implies "escalation", so usually we look for *another* node or just any high value node.
        // Let's check if currentId is high value.
        if (isHighValueTarget(nodes[currentId])) {
            if (currentId != startId) {
                targetId = currentId;
                break;
            }
        }

        if (adjacencyList.find(currentId) != adjacencyList.end()) {
            for (const auto& edge : adjacencyList[currentId]) {
                if (visited.find(edge.targetId) == visited.end()) {
                    visited.insert(edge.targetId);
                    parentEdge[edge.targetId] = edge;
                    q.push(edge.targetId);
                }
            }
        }
    }

    if (!targetId.empty()) {
        vector<Edge> path;
        string curr = targetId;
        while (curr != startId) {
            Edge e = parentEdge[curr];
            path.push_back(e);
            curr = e.sourceId;
        }
        reverse(path.begin(), path.end());
        return path;
    }

    return {};
}
