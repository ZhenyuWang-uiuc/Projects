#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../Graph.h"
#include "../Dijkstra.cpp"
#include <algorithm>
#include <cmath>

using std::string;
using std::vector;
using std::pair;
using std::round;

const static std::vector<std::string> fnames = {
    "myTest/Graph_1.txt",
    "myTest/Graph_2.txt",
    "myTest/Graph_3.txt",
    "myTest/Graph_4.txt",
    "myTest/Graph_5.txt",
    "myTest/AptInfo.txt",
};

const static std::vector<std::string> ptsName = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};

double simpleDist(int f_x, int f_y, int s_x, int s_y)
{
    return sqrt((f_x - s_x) * (f_x - s_x) + (f_y - s_y) * (f_y - s_y));
}

TEST_CASE("test_Graph_1", "[weight=10][timeout=8000]") {
    Graph graph(fnames[2], fnames[5]);
    graph.addPoints("A", "J");
    graph.createGraph(); 
    for (auto it = ptsName.begin(); it != ptsName.end(); ++it) {
        vector<string> ans = graph.getDestinations(*it);
        REQUIRE(ans.size() == 0);
    }
    REQUIRE(!graph.isReachable());
}

TEST_CASE("test_Graph_2", "[weight=10][timeout=8000]") {
    std::vector<unsigned> size = {5, 2, 3, 3, 4, 2, 3, 3, 1, 0};
    std::vector<std::vector<std::string>> children = {
        {"B", "D", "E", "H", "I"},
        {"A", "C"},
        {"B", "D", "F"},
        {"A", "C", "E"},
        {"A", "D", "G", "H"},
        {"C", "G"},
        {"E", "F", "H"},
        {"A", "E", "G"},
        {"A"},
        {}
    };
    Graph graph(fnames[0], fnames[5]);
    graph.addPoints("A", "J");
    graph.createGraph();
    for (unsigned i = 0; i < ptsName.size(); ++i) {
        vector<string> ans = graph.getDestinations(ptsName[i]);
        REQUIRE(ans.size() == size[i]);
        std::sort(ans.begin(), ans.end());
        REQUIRE(ans == children[i]);
    }
    REQUIRE(!graph.isReachable());
}

TEST_CASE("test_Graph_3", "[weight=10][timeout=8000]") {
    std::vector<unsigned> size = {2, 3, 3, 3, 2, 2, 2, 1, 1, 0};
    std::vector<std::vector<std::string>> children = {
        {"B", "C"},
        {"A", "D", "I"},
        {"A", "D", "J"},
        {"B", "C", "E"},
        {"D", "F"},
        {"E", "G"},
        {"F", "H"},
        {"G"},
        {"B"},
        {}
    };
    Graph graph(fnames[1], fnames[5]);
    graph.addPoints("A", "J");
    graph.createGraph(); 
    for (unsigned i = 0; i < ptsName.size(); ++i) {
        vector<string> ans = graph.getDestinations(ptsName[i]);
        REQUIRE(ans.size() == size[i]);
        std::sort(ans.begin(), ans.end());
        REQUIRE(ans == children[i]);
    }
    REQUIRE(graph.isReachable());
}

TEST_CASE("test_Graph_4", "[weight=10][timeout=8000]") {
    std::vector<unsigned> size = {2, 2, 1, 2, 1, 2, 2, 1, 1, 0};
    std::vector<std::vector<std::string>> children = {
        {"B", "F"},
        {"C", "D"},
        {"E"},
        {"F", "G"},
        {"G"},
        {"D", "H"},
        {"D", "I"},
        {"G"},
        {"J"},
        {}
    };
    Graph graph(fnames[3], fnames[5]);
    graph.addPoints("A", "J");
    graph.createGraph();  
    for (unsigned i = 0; i < ptsName.size(); ++i) {
        vector<string> ans = graph.getDestinations(ptsName[i]);
        REQUIRE(ans.size() == size[i]);
        std::sort(ans.begin(), ans.end());
        REQUIRE(ans == children[i]);
    }
    REQUIRE(graph.isReachable());
}

TEST_CASE("test_Graph_5", "[weight=10][timeout=8000]") { // test disconnected graph
    std::vector<unsigned> size = {2, 2, 2, 0, 0, 0, 0, 0, 0, 0};
    std::vector<std::vector<std::string>> children = {
        {"B", "C"},
        {"A", "C"},
        {"A", "B"},
        {},
        {},
        {},
        {},
        {},
        {},
        {}
    };
    Graph graph(fnames[4], fnames[5]);
    graph.addPoints("A", "J");
    graph.createGraph();
    for (unsigned i = 0; i < ptsName.size(); ++i) {
        vector<string> ans = graph.getDestinations(ptsName[i]);
        REQUIRE(ans.size() == size[i]);
        std::sort(ans.begin(), ans.end());
        REQUIRE(ans == children[i]);
    }
    REQUIRE(!graph.isReachable());
}

TEST_CASE("test_Graph_6", "[weight=10][timeout=8000]") {
    std::vector<unsigned> size = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<std::vector<std::string>> children = {
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {}
    };
    Graph graph(fnames[0], fnames[5]);
    graph.addPoints("X", "J");
    graph.createGraph();
    for (unsigned i = 0; i < ptsName.size(); ++i) {
        vector<string> ans = graph.getDestinations(ptsName[i]);
        REQUIRE(ans.size() == size[i]);
        std::sort(ans.begin(), ans.end());
        REQUIRE(ans == children[i]);
    }
    REQUIRE(!graph.isReachable());
}

TEST_CASE("test_Constructor_1", "[weight=10][timeout=8000]") {
    std::vector<unsigned> size = {2, 3, 3, 3, 2, 2, 2, 1, 1, 0};
    std::vector<std::vector<std::string>> children = {
        {"B", "C"},
        {"A", "D", "I"},
        {"A", "D", "J"},
        {"B", "C", "E"},
        {"D", "F"},
        {"E", "G"},
        {"F", "H"},
        {"G"},
        {"B"},
        {}
    };
    Graph graph(fnames[1], fnames[5]);
    graph.addPoints("A", "J");
    graph.createGraph();
    Graph graph2(graph);
    for (unsigned i = 0; i < ptsName.size(); ++i) {
        vector<string> ans = graph2.getDestinations(ptsName[i]);
        REQUIRE(ans.size() == size[i]);
        std::sort(ans.begin(), ans.end());
        REQUIRE(ans == children[i]);
    }
    REQUIRE(graph2.isReachable());
}

TEST_CASE("test_Assignment_Operator_1", "[weight=10][timeout=8000]") {
    std::vector<unsigned> size = {2, 3, 3, 3, 2, 2, 2, 1, 1, 0};
    std::vector<std::vector<std::string>> children = {
        {"B", "C"},
        {"A", "D", "I"},
        {"A", "D", "J"},
        {"B", "C", "E"},
        {"D", "F"},
        {"E", "G"},
        {"F", "H"},
        {"G"},
        {"B"},
        {}
    };
    Graph graph(fnames[1], fnames[5]);
    graph.addPoints("A", "J");
    graph.createGraph();
    Graph graph2(fnames[0], fnames[5]);
    graph2 = graph;
    for (unsigned i = 0; i < ptsName.size(); ++i) {
        vector<string> ans = graph2.getDestinations(ptsName[i]);
        REQUIRE(ans.size() == size[i]);
        std::sort(ans.begin(), ans.end());
        REQUIRE(ans == children[i]);
    }
    REQUIRE(graph2.isReachable());
}

TEST_CASE("test_Assignment_Operator_2", "[weight=10][timeout=8000]") {
    std::vector<unsigned> size = {2, 3, 3, 3, 2, 2, 2, 1, 1, 0};
    std::vector<std::vector<std::string>> children = {
        {"B", "C"},
        {"A", "D", "I"},
        {"A", "D", "J"},
        {"B", "C", "E"},
        {"D", "F"},
        {"E", "G"},
        {"F", "H"},
        {"G"},
        {"B"},
        {}
    };
    Graph graph(fnames[0], fnames[5], std::pair<string, string>("B", "J"));
    graph.createGraph();
    Graph graph2(fnames[1], fnames[5], std::pair<string, string>("A", "J"));
    graph2.createGraph();
    graph = graph2;
    graph2 = graph;
    for (unsigned i = 0; i < ptsName.size(); ++i) {
        vector<string> ans = graph2.getDestinations(ptsName[i]);
        REQUIRE(ans.size() == size[i]);
        std::sort(ans.begin(), ans.end());
        REQUIRE(ans == children[i]);
    }
    REQUIRE(graph2.isReachable());
}

TEST_CASE("test_Assignment_Operator_and_Copy_Constructor_1", "[weight=10][timeout=8000]") {
    std::vector<unsigned> size = {2, 3, 3, 3, 2, 2, 2, 1, 1, 0};
    std::vector<std::vector<std::string>> children = {
        {"B", "C"},
        {"A", "D", "I"},
        {"A", "D", "J"},
        {"B", "C", "E"},
        {"D", "F"},
        {"E", "G"},
        {"F", "H"},
        {"G"},
        {"B"},
        {}
    };
    Graph graph(fnames[0], fnames[5], std::pair<string, string>("X", "A"));
    graph.createGraph();
    Graph graph2(graph);
    Graph graph3(fnames[1], fnames[5], std::pair<string, string>("A", "J"));
    graph2 = graph3;
    graph2.createGraph();
    for (unsigned i = 0; i < ptsName.size(); ++i) {
        vector<string> ans = graph2.getDestinations(ptsName[i]);
        REQUIRE(ans.size() == size[i]);
        std::sort(ans.begin(), ans.end());
        REQUIRE(ans == children[i]);
    }
    REQUIRE(graph2.isReachable());
}

TEST_CASE("test_Dijkstra_Algorithm_1", "[weight=10][timeout=8000]") {
    Graph graph("myTest/Graph_6.txt", "myTest/AptInfo_2.txt", std::pair<string, string>("A", "H"));
    vector<string> expected = {"H", "E", "C", "A"};
    double distance = 10;
    graph.createGraph();
    REQUIRE(graph.isReachable());
    graph.AddCalcDistMethod(simpleDist);
    pair<vector<string>, double> ans = Dijkstra(graph);
    REQUIRE(ans.first == expected);
    REQUIRE(round(ans.second) == distance);
}

TEST_CASE("test_Dijkstra_Algorithm_2", "[weight=10][timeout=8000]") {
    Graph graph("myTest/Graph_6.txt", "myTest/AptInfo_2.txt", std::pair<string, string>("A", "F"));
    vector<string> expected = {"F", "A"};
    double distance = 4;
    graph.createGraph();
    REQUIRE(graph.isReachable());
    graph.AddCalcDistMethod(simpleDist);
    pair<vector<string>, double> ans = Dijkstra(graph);
    REQUIRE(ans.first == expected);
    REQUIRE(round(ans.second) == distance);
}

TEST_CASE("test_Dijkstra_Algorithm_3", "[weight=10][timeout=8000]") {
    Graph graph("myTest/Graph_6.txt", "myTest/AptInfo_2.txt", std::pair<string, string>("A", "J"));
    vector<string> expected = {"J", "B", "A"};
    double distance = 6;
    graph.createGraph();
    REQUIRE(graph.isReachable());
    graph.AddCalcDistMethod(simpleDist);
    pair<vector<string>, double> ans = Dijkstra(graph);
    REQUIRE(ans.first == expected);
    REQUIRE(round(ans.second) == distance);
}
