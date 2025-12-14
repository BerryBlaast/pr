#include "Graph.h"
#include <queue>
#include <algorithm>
#include <unordered_set>

void Graph::addNode(const std::string& id, const std::string& type, const std::string& name, const std::unordered_map<std::string, std::string>& properties) {
    nodes[id] = {id, type, name, properties};
}

void Graph::addEdge(const std::string& sourceId, const std::string& targetId, const std::string& type) {
    adjacencyList[sourceId].push_back({sourceId, targetId, type});
    reverseAdjacencyList[targetId].push_back({sourceId, targetId, type});
}

std::string Graph::findNodeIdByName(const std::string& name) {
    for (const auto& pair : nodes) {
        if (pair.second.name == name) {
            return pair.first;
        }
    }
    return "";
}

std::vector<std::string> Graph::shortestPath(const std::string& sourceName, const std::string& targetName) {
    std::string startId = findNodeIdByName(sourceName);
    std::string endId = findNodeIdByName(targetName);

    if (startId.empty()) {
        std::cerr << "Source node not found: " << sourceName << std::endl;
        return {};
    }
    if (endId.empty()) {
        std::cerr << "Target node not found: " << targetName << std::endl;
        return {};
    }

    std::queue<std::string> q;
    q.push(startId);

    std::unordered_map<std::string, std::string> parent; // To reconstruct path: child -> parent
    std::unordered_map<std::string, std::string> edgeType; // To store edge type: child -> type from parent
    std::unordered_set<std::string> visited;

    visited.insert(startId);

    bool found = false;

    while (!q.empty()) {
        std::string currentId = q.front();
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
        std::vector<std::string> path;
        std::string curr = endId;
        while (curr != startId) {
            std::string p = parent[curr];
            std::string type = edgeType[curr];
            std::string nodeName = nodes[curr].name;
            std::string nodeType = nodes[curr].type;
            
            path.push_back("(" + type + ") -> " + nodeName + " [" + nodeType + "]");
            curr = p;
        }
        path.push_back(nodes[startId].name + " [" + nodes[startId].type + "]");
        std::reverse(path.begin(), path.end());
        return path;
    }

    return {};
}
std::vector<std::string> Graph::getReachableNodes(const std::string& sourceName) {
    std::string startId = findNodeIdByName(sourceName);
    if (startId.empty()) {
        std::cerr << "Source node not found: " << sourceName << std::endl;
        return {};
    }

    std::vector<std::string> result;
    std::unordered_set<std::string> visited;
    std::queue<std::string> q;

    q.push(startId);
    visited.insert(startId);

    while (!q.empty()) {
        std::string currentId = q.front();
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

std::vector<std::string> Graph::getControllers(const std::string& targetName) {
    std::string targetId = findNodeIdByName(targetName);
    if (targetId.empty()) {
        std::cerr << "Target node not found: " << targetName << std::endl;
        return {};
    }

    std::vector<std::string> result;
    std::unordered_set<std::string> visited;
    std::queue<std::string> q;

    q.push(targetId);
    visited.insert(targetId);

    while (!q.empty()) {
        std::string currentId = q.front();
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
        if (node.name.find("DOMAIN ADMINS") != std::string::npos ||
            node.name.find("ENTERPRISE ADMINS") != std::string::npos ||
            node.name.find("SCHEMA ADMINS") != std::string::npos) {
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
            if (node.properties.at("distinguishedname").find("OU=Domain Controllers") != std::string::npos) {
                return true;
            }
        }
        // Fallback to naming convention heuristic
        if (node.name.find("DC") != std::string::npos) { // Simple heuristic
            return true;
        }
    }
    return false;
}

std::vector<Edge> Graph::findPathToHighValue(const std::string& sourceName) {
    std::string startId = findNodeIdByName(sourceName);
    if (startId.empty()) {
        std::cerr << "Source node not found: " << sourceName << std::endl;
        return {};
    }

    std::queue<std::string> q;
    q.push(startId);

    std::unordered_map<std::string, Edge> parentEdge; // childId -> Edge from parent
    std::unordered_set<std::string> visited;
    visited.insert(startId);

    std::string targetId = "";

    while (!q.empty()) {
        std::string currentId = q.front();
        q.pop();

        // Check if current node is high value (but not the start node itself, unless it's a path to itself? No, usually we want escalation)
        // Actually, if we start at a high value node, the path is size 0 or we can say we are already there.
        // But the prompt implies "escalation", so usually we look for *another* node or just any high value node.
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
        std::vector<Edge> path;
        std::string curr = targetId;
        while (curr != startId) {
            Edge e = parentEdge[curr];
            path.push_back(e);
            curr = e.sourceId;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    return {};
}
