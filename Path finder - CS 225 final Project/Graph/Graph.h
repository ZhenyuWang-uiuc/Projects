/*
 * Author: Zhenyu Wang
 * Date: 4/20/2021
 */

#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <vector>

using namespace std;

class Graph {
   
    /* Node class definition */
    struct Node {
        Node(string name) : name(name) {}
        string name;
        unordered_map<string, Node*> children;
    };
 
    public:
    
        /**
         * Constructor
         * @param tixInfo string that contains the name of tixInfo
         * @param aptInfo string that contains the name of arpnfo
         * @param pts pair which contains starting point  & end point
         */
        Graph(const string & tixInfo, const string & aptInfo, const pair<string, string> & pts);
    
        /**
         * Constructor
         * @param tixInfo string that contains the name of tixInfo
         * @param aptInfo string that contains the name of arpnfo
         */
        Graph(const string & tixInfo, const string & aptInfo);
    
        /* Destructor */
        ~Graph();
        
        /* Copy Constructor */
        Graph(const Graph &);
    
        /* Assignment Operator */
        Graph & operator=(const Graph &);
    
        /**
         * AddCalcDistMethod - adding a method to calcluate distances between two airports
         * @param f function that uses to calcuate distance bwtween two pts
         */
        void AddCalcDistMethod(const function<double(double, double, double, double)> f);
    
        /**
         * CreateGraph - try to create a graph
         * @return a string that contains 3 possible answer - "success", 
         *                                                  - "can't read airports information", 
         *                                                  - "can't read distance information"
         */
        string createGraph();
    
        /**
         * addPoints - try to change or add start pt and end pt
         * @param start string that contains the name of start airport
         * @param end string that contains the name of end airport
         */
        void addPoints(const string & start, const string & end);
    
        /**
         * getDestinations - get all of the current airport's children's name
         * @param name string that specifiy which airport's children that I want to know
         */
        vector<string> getDestinations(const string & name);

        /**
         * isReachable - to know that if the destination is reachable or not. If the destination is unreachable
         * don't have to traverse the graph
         */ 
        bool isReachable() const;

        /**
         * DFS - Use DFS algorithm to find a path between starting pt and end pt
         */
        vector<string> DFS();

        /**
         * IDDFS - Use IDDFS algorithm to find a path between starting pt and end pt
         */
        friend void IDDFS(string start, string sss, int DB, Graph graf);

        /**
         * IDDFS - Use Dijkstra algorithm to find a path between starting pt and end pt
         */
        friend pair<vector<string>, double> Dijkstra(Graph graph);
        
        /**
         * distance - save the distance of each path
         */ 
        friend double distance(Graph gg);
         
    private:
        /* private members */
        bool reachable = false;                                      // whether the destination is reachable or not, by default, no
        string end_;                                                 // end point
        string start_;                                               // start point
        string nameOfTixInfo_;                                       // name of the ticket information file
        string nameOfAptInfo_;                                       // name of the airport information file
        fstream tixInfo_;                                            // file contains tickets infomation
        fstream aptInfo_;                                            // file contains airport infomation
        unordered_map<string, pair<double, double>> locations_;      // airports' locations
        unordered_map<string, Node*> pts_;                           // including all Nodes;
        
        function<double(double, double, double, double)> calcDist_;  // the function that uses to calculate distance
    
        /* private functions */
        void _free();                            /* helper function that uses to free all Nodes */
        void _copy(const Graph & obj);           /* helper function that uses to copy info from another Graph */
        void _readLocations();                   /* helper function that reads all the airports' locations */
        void _graphGenerator();                  /* real creating graph algorithm */
        void _DFS_Helper(Node *& cur, vector<string> & ans, unordered_set<string> & visited);  /* DFS Helper */
};
