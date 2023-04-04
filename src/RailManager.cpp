
#include <stdexcept>

#include "RailManager.h"

using namespace std;


RailManager::RailManager() = default;

RailManager::RailManager(const string& datasetPath) {

    initializeStations(CSVReader::read(datasetPath + "stations.csv"));
    initializeSegments(CSVReader::read(datasetPath + "network.csv"));
    initializeNetwork();
}

void RailManager::addSegment(const string& stationA, const string& stationB, unsigned int capacity, SegmentType service) {
    segments[stationA].insert({stationB, Segment(stationA, stationB, capacity, service)});
    segments[stationB].insert({stationA, Segment(stationB, stationA, capacity, service)});
}

void RailManager::addStation(const string& name, const string& district, const string& municipality, const string& township, const string& line){
    stations.insert({name, Station(name, district, municipality, township, line)});
}

const Segment& RailManager::getSegment(const string &origin, const string &destination) {
    return segments.at(origin).at(destination);
}

const Station &RailManager::getStation(const std::string &station) {
    return stations.at(station);
}

void RailManager::initializeStations(const CSV &stationsCSV) {
    for(int i = 1; i < stationsCSV.size(); i++){ // Skip first line
        CSVLine line = stationsCSV[i];
        if(line.size() != 5) throw std::exception();
        addStation(line[0], line[1], line[2], line[3], line[4]);
    }
}

void RailManager::initializeSegments(const CSV &networkCSV) {
    for(int i = 1; i < networkCSV.size(); i++){ // Skip first line
        CSVLine line = networkCSV[i];
        if(line.size() != 5) throw std::exception();
        SegmentType service;
        if (line[3] == "STANDARD") service = STANDARD;
        else if (line[3] == "ALFA PENDULAR") service = ALFA_PENDULAR;
        else service = INVALID;
        if (service) addSegment(line[0], line[1], stoi(line[2]), service);
    }
}

void RailManager::initializeNetwork() {
    for (const auto& [name, station] : stations) {
        list<RailNetwork::Edge> l;
        for (const auto& [dest, seg] : segments.at(name))
            l.emplace_back(name, dest, seg.capacity);
        railNet.addNode(name, l);
    }
}

