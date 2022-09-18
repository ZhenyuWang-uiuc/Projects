/*
 * Author: Zhenyu Wang
 * Date: 4/20/2021
 */

#include "Graph.h"

using namespace std;

/**
 * Constructor
 * @param tixInfo string that contains the name of tixInfo
 * @param aptInfo string that contains the name of aptInfo
 * @param pts pair which contains starting point & end point
 */
Graph::Graph(const string & tixInfo, const string & aptInfo, const pair<string, string> & pts)
{
    end_           = pts.second;
    nameOfTixInfo_ = tixInfo;
    start_         = pts.first;
    nameOfAptInfo_ = aptInfo;
}

/**
 * Constructor
 * @param tixInfo string that contains the name of tixInfo
 * @param aptInfo string that contains the name of arpnfo
 */
Graph::Graph(const string & tixInfo, const string & aptInfo)
{
    nameOfTixInfo_ = tixInfo;
    nameOfAptInfo_ = aptInfo;
}

/**
 * Destructor
 */
Graph::~Graph()
{
    _free();
}

/**
 * Copy Constructor
 */
Graph::Graph(const Graph & obj)
{
    _copy(obj);
}

/**
 * Assignment Operator
 */
Graph & Graph::operator=(const Graph & rhs)
{
    if (this != &rhs) {
        _free();
        _copy(rhs);
    }
    return *this;
}

/**
 * _copy - helper function that uses to copy info from another Graph
 */
void Graph::_copy(const Graph & obj)
{
    start_         = obj.start_;
    end_           = obj.end_;
    calcDist_      = obj.calcDist_;
    locations_     = obj.locations_;
    nameOfAptInfo_ = obj.nameOfAptInfo_;
    nameOfTixInfo_ = obj.nameOfTixInfo_;
    reachable      = obj.reachable;

    // copy all node information
    for (pair<string, Node*> p : obj.pts_) {
        if (pts_.find(p.first) == pts_.end())
            pts_[p.first] = new Node(p.first);

        // copy children
        for (auto it = p.second->children.begin(); it != p.second->children.end(); ++it) {
            if (pts_.find((*it).first) == pts_.end()) {  /* can't find this "child" */
                Node * tmp = new Node((*it).first);         // step 1 - create a node
                pts_[p.first]->children[tmp->name] = tmp;   // step 2 - create a path
                pts_[tmp->name] = tmp;                      // step 3 - add node to pts_
            } else {  /* otherwsie, create a path immediately */
                pts_[p.first]->children[(*it).first] = pts_[(*it).first];
            }
        }
    }
}

/**
 * _free - helper function that uses to free all Nodes
 */
void Graph::_free()
{
    if (!pts_.empty()) {
        for (auto it = pts_.begin(); it != pts_.end(); ++it) {
            delete it->second;
        }
    }
    pts_.clear();
}

/**
 * AddCalcDistMethod - adding a method to calcluate distances between two airports
 * @param f function that uses to calcuate distance bwtween two pts
 */
void Graph::AddCalcDistMethod(const function<double(double, double, double, double)> f)
{
    calcDist_ = f;
}

/**
 * createGraph - try to create a graph
 * @return a string that contains 3 possible answer - "success", 
 *                                                  - "can't read airports information", 
 *                                                  - "can't read distance information"
 */
string Graph::createGraph()
{
    if (!tixInfo_) { // check whether both files open successfully
        return "can't read distance information";
    } else if (!aptInfo_) {
        return "can't read airports information";
    } else { // create a graph
        tixInfo_.open(nameOfTixInfo_, ios::in);
        aptInfo_.open(nameOfAptInfo_, ios::in);
        _graphGenerator();
        _readLocations();
        tixInfo_.close();
        aptInfo_.close();
        return "success";
    }
}

/**
 * _readLocations - helper function that reads all the airports' locations
 */
void Graph::_readLocations()
{
    string line, tmp, name, lon, lat;  // line of data, segement of the line, longitude, latitude
    int c = 0;                         // counter
    bool isFinded = false;             // determine whether the airport is part of our graph
    
    while (getline(aptInfo_, line)) {
        while (getline(stringstream(line), tmp, ',')) {
            if (c == 4 && tmp == "\\N") {
                break;
            } else if (c == 4) { // check each airport's IATA code and decide whether I need to continue
                name = tmp;
                name.erase(name.begin());
                name.erase(name.begin() + name.size() - 1);
                if (pts_.find(name) == pts_.end()) { break; }
            } else if (c == 6) {
                lon = tmp;
            } else if (c == 7) { // get pt's latitude and finish current search
                lat = tmp; isFinded = true; break;
            }
            line.erase(0, tmp.length() + 1);
            c++;
        }
        if (name != "\\N" && isFinded)
            locations_[name] = pair<double, double>(stod(lon), stod(lat));
        isFinded = false;
        c = 0;
    }
}

/**
 * _graphGenerator -  real creating graph algorithm
 */
void Graph::_graphGenerator()
{
    int counter = 0;
    string line, tmp;        // line of data and segment of the line
    
    while (getline(tixInfo_, line)) {
        vector<string> info;
        while (getline(stringstream(line), tmp, ',')) {
            if (counter > 2)
                break;
            else if (counter % 2 == 0)
                info.push_back(tmp);
            line.erase(0, tmp.length() + 1);
            ++counter;
        }
        counter = 0;
        
        // create a path
        if (info[0].length() == 3 && pts_.find(info[0]) == pts_.end()) {
            pts_[info[0]] = new Node(info[0]);
        }
        if (info[1].length() == 3 && pts_.find(info[1]) == pts_.end()) {
            pts_[info[1]] = new Node(info[1]);
        }
        pts_[info[0]]->children[info[1]] = pts_[info[1]];
    }
}

/**
 * addPoints - try to change or add start pt and end pt
 * @param start string that contains the name of start airport
 * @param end string that contains the name of end airport
 */
void Graph::addPoints(const string & start, const string & end)
{
    start_ = start;
    end_   = end;
}

/**
 * getDestinations - get all of the current airport's children
 * @param name string that specifiy which airport's children that I want to know
 */
vector<string> Graph::getDestinations(const string & name)
{
    auto it = pts_.find(name);
    if (it == pts_.end())
        return vector<string>();

    vector<string> ans;
    for (auto iterator = pts_[name]->children.begin(); iterator != pts_[name]->children.end(); ++iterator)
        ans.push_back((*iterator).first);
    return ans;
}

/**
 * isReachable - to know that if the destination is reachable or not. If the destination is unreachable
 * don't have to traverse the graph
 */
bool Graph::isReachable() const
{
    return true;
}

/**
 * DFS - Use DFS algorithm to find a path between starting pt and end pt
 */
vector<string> Graph::DFS()
{
    unordered_set<string> visited( {start_} );
    vector<string> ans(1, start_);
    _DFS_Helper(pts_[start_], ans, visited);
    return ans;
}

/** 
 * DFS Helper 
 */
void Graph::_DFS_Helper(Graph::Node *& cur, vector<string> & ans, unordered_set<string> & visited)
{
    if (cur->name == this->end_) {
        return;
    } else {
        for (auto it = cur->children.begin(); it != cur->children.end(); ++it) {
            if (visited.find((*it).first) == visited.end()) {
                ans.push_back((*it).first);
                visited.insert((*it).first);
                _DFS_Helper((*it).second, ans, visited);
                if (find(ans.begin(), ans.end(), this->end_) != ans.end())
                    return;
                visited.erase((*it).first);
                ans.pop_back();
            }
        }
    }
}

