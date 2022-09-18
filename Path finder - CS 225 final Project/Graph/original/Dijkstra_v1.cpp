#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "Graph.h"

using namespace std;

int Dijkstra(string start, string dest, Graph graph)  {

// handout pseudocode

int initialDist = 9999999;
unordered_map<string,int> distance;
unordered_map<string,int> predecessor;

  for(auto vertex : pts_) {
    distance[vertex] = initialDist;
    predecessor[vertex] = NULL;
  }

  distance[start] = 0;

priority_queue<pair<string,int>> Q;

  Q.push({0,s});
  
  while(!Q.empty()) {
    string next;
    next = Q.top().second;
    Q.pop();
    
    if(predecessor[next] = true)
      continue;
    
    processed[next] = true;
    
    for(auto x : pts_)  {
      string curVer = x.first;
      int curDist = x.second;
      
      if(distance[next] + curDist < distance[curVer]) {
        distance[curVer] = distance[next] + curDist;
        Q.push({distance[curVer, curVer]});
      }
    }
  }
  return distance[dest];
}