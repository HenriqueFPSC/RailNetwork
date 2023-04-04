
#include "RailNetwork.h"
#include "Segment.h"

using namespace std;


const RailNetwork::Node& RailNetwork::getNode(const string &station) {
    return nodes.at(station);
}

void RailNetwork::addNode(const std::string& name, const std::list<Edge>& adj) {
    nodes.insert({name, Node(name, "", adj, false)});
}

void RailNetwork::clearVisits() {
    for(auto& p : nodes)
        p.second.visited = false;
}
void RailNetwork::visit(const std::string& station){
    nodes.at(station).visited = true;
}

list<string> RailNetwork::BFS(const string& src, const string& dest){
    clearVisits();
    queue<string> q;
    q.push(src);
    list<string> res;
    visit(src);
    bool found = false;
    while(!q.empty() ){ // No more Nodes
        string curr = q.front();
        visit(curr);
        list<Edge> edges = getAdj(curr);
        q.pop();
        for(const Edge& edge : edges) {
            Node aux = nodes.at(edge.dest);
            if(!aux.visited){
                aux.prev = curr;
                q.push(edge.dest);
            }
            if(edge.dest == dest){
                found = true;
                break;
            }
        }
        if(found) break;
    }
    if(found){
        res.push_front(dest);
        while(true) {
            if (res.front() != src) {
                res.push_front(nodes.at(res.front()).prev);
            }
            else break;
        }
    }
    return res;
}
list<std::string> RailNetwork::BFSFlow(const string &src, const string &dest) {
    clearFlow();
    clearVisits();
    queue<string> q;
    q.push(src);
    list<string> res;
    visit(src);
    bool found = false;
    while(!q.empty() ){ // No more Nodes
        string curr = q.front();
        visit(curr);
        list<Edge> edges = getAdj(curr);
        q.pop();
        for(const Edge& edge : edges) {
            if(edge.flow == edge.capacity) continue; // if segment flow is full dont add node to queue
            Node aux = nodes.at(edge.dest);
            if(!aux.visited){
                aux.prev = curr;
                q.push(edge.dest);
            }
            if(edge.dest == dest){
                found = true;
                break;
            }
        }
        if(found) break;
    }
    if(found){
        res.push_front(dest);
        while(true) {
            if (res.front() != src) {
                res.push_front(nodes.at(res.front()).prev);
            }
            else break;
        }
    }
    return res;
}
list<RailNetwork::Edge> RailNetwork::getAdj(const string &station) {
    return nodes.at(station).adj;
}

// []===========================================[] //
// ||          ALGORITHMIC FUNCTIONS            || //
// []===========================================[] //

int RailNetwork::maxFlow(const string &origin, const string &destination) {
    int maxFlow=0;
    while(true){
        list<string> res = BFSFlow(origin,destination);
        if(res.empty()) break;
        int bottleneck = INT_MAX;
        auto stop = res.end();
        stop--;
        for(auto it= res.begin(); it != stop; it++){ // find bottleneck in the shortest path
            Node ver=nodes.at(*it);
            auto iter2 = it;
            iter2++;
            string next = *iter2;
            Edge edge = getEdge(*it,next);
            unsigned remaining = edge.capacity - edge.flow;
            if(remaining < bottleneck)
                bottleneck = remaining;
        }
        maxFlow+=bottleneck;
        for(auto it=res.begin();it!=stop;it++){ // find bottleneck in the shortest path
            Node ver=nodes.at(*it);
            auto iter2 = it;
            iter2++;
            string next = *iter2;
            Edge edge = getEdge(*it,next);
            edge.flow += bottleneck;
        }
    }
    return maxFlow;
}

list<RailNetwork::Edge> RailNetwork::importantEdges() {
    // TODO: [2.2]
    list<Edge> edges;
    for (const auto& [origin, node] : nodes) {
        for (const auto& edge : node.adj) {
            int flow = maxFlow(origin, edge.dest);
            if (flow > 0) {
                edges.push_back(edge);
            }
        }
    }
    edges.sort();
    return edges;
}

std::list<std::string> RailNetwork::topMunicipalities(int k) {
    // TODO: [2.3]
    return std::list<std::string>();
}

int RailNetwork::maxFlowStation(const string &station) {
    // TODO: [2.4]
    return 0;
}

int RailNetwork::maxFlowMinCost(const string &origin, const string &destination) {
    // TODO: [3.1]
    return 0;
}

int RailNetwork::maxFlowReduced(const string &origin, const string &destination) {
    // TODO: [4.1]
    return 0;
}

std::list<std::string> RailNetwork::topAffectedStations(int k) {
    // TODO: [4.2]
    return std::list<std::string>();
}

RailNetwork::Edge &RailNetwork::getEdge(const std::string src, const string &dest) {
    for(Edge& edge:nodes.at(src).adj){
        if(edge.dest==dest)
            return edge;
    }
}

void RailNetwork::clearFlow() {
    for(auto& [name,node]:nodes){
        for(Edge& e:node.adj)
            e.flow=0;
    }
}


