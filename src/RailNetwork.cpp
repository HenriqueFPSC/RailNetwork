
#include <stdexcept>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <iostream>

#include "RailNetwork.h"
#include "Segment.h"

using namespace std;


// Compare Classes for Priority Queues //
template<class T>
class MaxHeapCompare {
public:
    bool operator() (const pair<T,unsigned>& p1, const pair<T, unsigned>& p2) {
        return p1.second > p2.second;
    }
};

template<class T>
class MinHeapCompare {
public:
    bool operator() (const pair<T,unsigned>& p1, const pair<T, unsigned>& p2) {
        return p1.second < p2.second;
    }
};


const std::string RailNetwork::sourceNodeName = "__SOURCE_NODE_NAME__";

RailNetwork::Node& RailNetwork::getNode(const string &station) {
    return nodes.at(station);
}

void RailNetwork::addNode(const std::string& name, const std::list<Edge>& adj) {
    nodes.insert({name, Node(name, adj)});
}

RailNetwork::Edge& RailNetwork::getEdge(const std::string& src, const string &dest) {
    for(Edge& edge:nodes.at(src).adj)
        if(edge.dest == dest)
            return edge;
    throw std::out_of_range("Didn't Find the Edge.");
}

void RailNetwork::visit(const std::string& station){
    nodes.at(station).visited = true;
}

void RailNetwork::clearVisits() {
    for(auto& p : nodes)
        p.second.visited = false;
}

void RailNetwork::clearFlow() {
    for(auto& [_,node] : nodes)
        for(Edge& e : node.adj)
            e.flow = 0;
}

void RailNetwork::clearCost() {
    for(auto& [_,node] : nodes)
        node.cost = UINT_MAX;
}

void RailNetwork::setCost(const string& node, unsigned cost) {
    nodes.at(node).cost = cost;
}

unsigned RailNetwork::getCost(const string& node) {
    return nodes.at(node).cost;
}

void RailNetwork::setPrev(const string &node, const string &prev) {
    nodes.at(node).prev = prev;
}

list<string> RailNetwork::BFSFlow(const string &src, const string &dest) {
    clearVisits();
    queue<pair<string, SegmentType>> q;
    q.push({src, INVALID});
    list<string> res;
    visit(src);
    while (!q.empty()) { // No more Nodes
        auto [curr, type] = q.front();
        visit(curr);
        list<Edge> edges = getAdj(curr);
        q.pop();
        for(const Edge& edge : edges) {
            if (type != INVALID && (type != edge.type)) continue; // Different Train
            if (edge.flow == edge.capacity) continue; // if segment flow is full dont add node to queue
            if (!getNode(edge.dest).visited) {
                setPrev(edge.dest, curr);
                q.push({edge.dest, edge.type});
            }
            if (edge.dest == dest) goto found_destination;
        }
    }
    goto skip_found_destination;
    found_destination:
    res.push_front(dest);
    while (true) {
        if (res.front() == src) break;
        res.push_front(getNode(res.front()).prev);
    }
    skip_found_destination:
    return res;
}

static unsigned getCostByType(SegmentType type){
    switch(type){
        case INVALID:
            return 0;
        case STANDARD:
            return 2;
        case ALFA_PENDULAR:
            return 4;
    }
}

list<list<string>> RailNetwork::BFSCost(const string &src, const string &dest) {
    clearCost();
    priority_queue<pair<pair<list<string>, SegmentType>, unsigned>, vector<pair<pair<list<string>, SegmentType>, unsigned>>, MinHeapCompare<pair<list<string>, SegmentType>>> q;
    q.push({{{src}, INVALID}, 0});
    setCost(src, 0);
    list<list<string>> res;
    bool found = false;
    unsigned minCost = UINT_MAX;
    while(!q.empty() ){ // No more Nodes
        auto [p, cost] = q.top();
        auto [curr, type] = p;
        q.pop();
        if (cost > minCost) break;
        if (curr.back() == dest) {
            res.push_back(curr);
            minCost = cost;
            continue;
        }
        list<Edge> edges = getAdj(curr.back());
        unsigned newCost = cost + getCostByType(type);
        for(const Edge& edge : edges) {
            if (!type && (type != edge.type)) continue; // Different Train
            if (newCost <= getCost(edge.dest)) { // Better Path
                setCost(edge.dest, newCost);
                curr.push_back(edge.dest);
                q.push({{curr, type}, newCost});
                curr.pop_back();
            }
        }
    }
    return res;
}

list<string> RailNetwork::BFSActive(const string &src, const string &dest) {
    clearVisits();
    queue<pair<string, SegmentType>> q;
    q.push({src, INVALID});
    list<string> res;
    visit(src);
    while(!q.empty() ){ // No more Nodes
        auto [curr, type] = q.front();
        visit(curr);
        list<Edge> edges = getAdj(curr);
        q.pop();
        for(const Edge& edge : edges) {
            if (!type && (type != edge.type)) continue; // Different Train
            if(!edge.active) continue; //if edge is deactivated
            if(!getNode(edge.dest).active) continue; // if destination station is deactivated
            if (edge.flow == edge.capacity) continue; // if segment flow is full dont add node to queue
            if(!getNode(edge.dest).visited){
                setPrev(edge.dest, curr);
                q.push({edge.dest, type});
            }
            if(edge.dest == dest) goto found_destination;
        }
    }
    goto skip_found_destination;
    found_destination:
    res.push_front(dest);
    while (true) {
        if (res.front() == src) break;
        res.push_front(getNode(res.front()).prev);
    }
    skip_found_destination:
    return res;
}

list<string> RailNetwork::distancedNodes(const string& src, unsigned distance) {
    clearVisits();
    queue<pair<string, unsigned>> q;
    q.push({src, 0});
    visit(src);
    list<string> res;
    while(!q.empty() ){ // No more Nodes
        auto [curr, dist] = q.front();
        q.pop();
        if (dist > distance) break; // No more distant nodes
        if (dist == distance) res.push_back(curr);
        visit(curr);
        for(const Edge& edge : getAdj(curr))
            if(!getNode(edge.dest).visited)
                q.push({edge.dest, dist + 1});
    }
    return res;
}

list<RailNetwork::Edge> RailNetwork::getAdj(const string &station) {
    return getNode(station).adj;
}

void RailNetwork::addEdge(const string &node, const Edge &edge) {
    getNode(node).adj.push_back(edge);
}

// []===========================================[] //
// ||          ALGORITHMIC FUNCTIONS            || //
// []===========================================[] //

unsigned RailNetwork::maxFlow(const string &origin, const string &destination) {
    // Exercise [2.1]
    clearFlow();
    unsigned maxFlow = 0;
    while(true){
        list<string> res = BFSFlow(origin,destination);
        if(res.empty()) break;
        unsigned bottleneck = UINT_MAX;
        auto it = res.begin();
        auto itNext = res.begin();
        itNext++;
        while(itNext != res.end()){ // find bottleneck in the shortest path
            const Edge& edge = getEdge(*it, *itNext);
            unsigned remaining = edge.capacity - edge.flow;
            if(remaining < bottleneck) bottleneck = remaining;
            it++; itNext++;
        }
        it = res.begin();
        itNext = res.begin();
        itNext++;
        maxFlow += bottleneck;
        while(itNext != res.end()){ // find bottleneck in the shortest path
            Edge& edge = getEdge(*it, *itNext);
            edge.flow += bottleneck;
            it++; itNext++;
        }
    }
    return maxFlow;
}

list<pair<string,string>> RailNetwork::importantStations() {
    // Exercise [2.2]
    int maxF = 0;
    list<pair<string, string>> pairs;
    for (const auto& [name1, _1] : nodes) {
        for (const auto& [name2, _2] : nodes) {
            if (name1 == name2) continue;
            unsigned flow = maxFlow(name1, name2);
            if (flow > maxF) pairs = list<pair<string, string>>{{name1, name2}};
            else if (flow == maxF) pairs.emplace_back(name1, name2);
        }
    }
    return pairs;
}

list<string> RailNetwork::topMunicipalities(int k, const unordered_map<string, Station>& stations) {
    // Exercise [2.3]
    // Where should management assign larger budgets?
    // Ans: To municipalities where there are more trains (Max Flow).
    unordered_map<string, RailNetwork> municipalities;
    for (auto& [name, node] : nodes) {
        const string& municipality = stations.at(name).municipality;
        municipalities[municipality].addNode(name, {});
        for (const auto& edge : node.adj)
            if (stations.at(edge.dest).municipality == municipality)
                addEdge(name, edge);
    }
    priority_queue<pair<string, unsigned>, vector<pair<string, unsigned>>, MaxHeapCompare<string>> munMaxFlows;
    for (auto& [municipality, graph] : municipalities) {
        priority_queue<pair<string, unsigned>, vector<pair<string, unsigned>>, MaxHeapCompare<string>> nodeDegrees;
        for (auto& [name, node] : graph.nodes) {
            unsigned degree = 0;
            for (auto& edge : node.adj)
                degree += edge.capacity;
            nodeDegrees.push({name, degree});
        }
        string origin = nodeDegrees.top().first;
        nodeDegrees.pop();
        string dest = nodeDegrees.top().first;
        unsigned maxFlow = graph.maxFlow(origin, dest);
        munMaxFlows.push({municipality, maxFlow});
    }
    list<string> res;
    for (int i = 0; i < k; i++) {
        if (munMaxFlows.empty()) break;
        res.push_back(munMaxFlows.top().first);
        munMaxFlows.pop();
    }
    return res;
}
list<string> RailNetwork::topDistricts(int k, const unordered_map<string, Station>& stations) {
    // Exercise [2.3]
    // Where should management assign larger budgets?
    // Ans: To districts where there are more trains (Max Flow).
    unordered_map<string, RailNetwork> districts;
    for (auto& [name, node] : nodes) {
        const string& district = stations.at(name).district;
        districts[district].addNode(name, {});
        for (const auto& edge : node.adj)
            if (stations.at(edge.dest).district == district)
                addEdge(name, edge);
    }
    priority_queue<pair<string, unsigned>, vector<pair<string, unsigned>>, MaxHeapCompare<string>> disMaxFlows;
    for (auto& [district, graph] : districts) {
        priority_queue<pair<string, unsigned>, vector<pair<string, unsigned>>, MaxHeapCompare<string>> nodeDegrees;
        for (auto& [name, node] : graph.nodes) {
            unsigned degree = 0;
            for (auto& edge : node.adj)
                degree += edge.capacity;
            nodeDegrees.push({name, degree});
        }
        string origin = nodeDegrees.top().first;
        nodeDegrees.pop();
        string dest = nodeDegrees.top().first;
        unsigned maxFlow = graph.maxFlow(origin, dest);
        disMaxFlows.push({district, maxFlow});
    }
    list<string> res;
    for (int i = 0; i < k; i++) {
        if (disMaxFlows.empty()) break;
        res.push_back(disMaxFlows.top().first);
        disMaxFlows.pop();
    }
    return res;
}
unsigned RailNetwork::maxFlowStation(const string &station) {
    // Exercise [2.4]
    list<string> nodesAtDistanceTwo = distancedNodes(station, 2);
    if (nodesAtDistanceTwo.empty()) {
        unsigned sum = 0;
        for (const Edge& edge : getAdj(station))
            sum += edge.capacity;
        return sum;
    }
    Node sourceNode = Node(sourceNodeName, {});
    nodes.insert({sourceNodeName, sourceNode});
    for (const string& node : nodesAtDistanceTwo) {
        addEdge(sourceNodeName, Edge(sourceNodeName, node, INVALID, UINT_MAX));
    }
    unsigned res = maxFlow(sourceNodeName, station);
    nodes.erase(sourceNodeName);
    return res;
}


unsigned RailNetwork::maxFlowMinCost(const string &origin, const string &destination) {
    // Exercise [3.1]
    // 1 - Get all paths with the minimum cost.
    // 2 - Build a sub-graph with only the nodes and edges that belong to any minCost path.
    // 3 - Calculate max flow of the sub-graph.
    // Step 1:
    list<list<string>> allPathsMinCost = BFSCost(origin, destination);
    // Step 2:
    RailNetwork subGraph;
    for (const list<string>& path : allPathsMinCost){
        string prev;
        for (const string& node : path) {
            if (!prev.empty()) {
                try {
                    subGraph.getEdge(prev, node);
                } catch (std::out_of_range& e) {
                    // If Edge not in subGraph add it.
                    subGraph.addEdge(prev, getEdge(prev, node));
                }
            }
            try {
                subGraph.getNode(node);
            } catch (std::exception& e) {
                // If Node not in subGraph add it.
                subGraph.addNode(node, {});
            }
            prev = node;
        }
    }
    // Step 3:
    return subGraph.maxFlow(origin, destination);
}

unsigned RailNetwork::maxFlowReduced(const string &origin, const string &destination) {
    clearFlow();
    unsigned maxFlow = 0;
    while(true){
        list<string> res = BFSActive(origin,destination);
        if(res.empty()) break;
        unsigned bottleneck = UINT_MAX;
        auto it = res.begin();
        auto itNext = res.begin();
        itNext++;
        while(itNext != res.end()){ // find bottleneck in the shortest path
            Node vert = nodes.at(*it);
            string next = *itNext;
            Edge edge = getEdge(*it,next);
            unsigned remaining = edge.capacity - edge.flow;
            if(remaining < bottleneck) bottleneck = remaining;
            it++; itNext++;
        }
        it = res.begin();
        itNext = res.begin();
        itNext++;
        maxFlow += bottleneck;
        while(it != res.end()){ // find bottleneck in the shortest path
            Node vert = nodes.at(*it);
            string next = *itNext;
            Edge edge = getEdge(*it,next);
            edge.flow += bottleneck;
            it++; itNext++;
        }
    }
    return maxFlow;
}

std::list<std::string> RailNetwork::topAffectedStations(int k) {
    // TODO: [4.2]
    return std::list<std::string>();
}
