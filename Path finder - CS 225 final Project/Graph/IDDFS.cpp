#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "dist.cpp" 
#include "Graph.h"
using namespace std;

/* GLobal Variables */
int counter = 0;
int counter2 = 0;
int depthbound = 0;
int best_i = 0;
stack<string> path;
vector<stack<string> > saved;

//function used to copy the stack in the parameter. returns the new copy stack
stack<string> copyStack(stack<string> copyThis)
{
    stack<string> copy_;
    stack<string> temp;
    while (!copyThis.empty()) {
    	temp.push(copyThis.top());
    	copyThis.pop();
    }
    while (!temp.empty()) {
    	copyThis.push(temp.top());
    	copy_.push(temp.top());
    	temp.pop();
    }
    return copy_;
}

//check if the two stacks in parameters have same contents. returns true if it is.
bool isEqual(stack<string> a, stack<string> b)
{
    stack<string> copyA = copyStack(a);
    stack<string> copyB = copyStack(b);

    if (copyA.size() != copyB.size())
    	return false;
    while (!copyA.empty()) {
        if (copyA.top() != copyB.top())
            return false;
        copyA.pop();
        copyB.pop();
    }
    return true;
}

//saves a path into the vector of all paths
void savePath(stack<string> saveThis)
{
    for (unsigned i=0; i<saved.size(); i++) {
        stack<string> comp = copyStack(saved[i]);
        stack<string> temp = copyStack(saveThis);
        if (isEqual(temp, comp)) {
            return;
        }
    }
    counter2++;
    stack<string> save_ = copyStack(saveThis);
    saved.push_back(save_);
}

//prints all the paths from the starting airport to ending airport
void printSaved(string og)
{
    for (unsigned i=0; i<saved.size(); i++) {
        stack<string> temp = copyStack(saved[i]);
        cout << "Possible path with " << temp.size()-2 << " stops: ";
        while (!temp.empty()) {
            if(temp.top() == og)
                cout << temp.top();
            else
                cout << temp.top() << " <-- ";
            temp.pop();
        }
        cout << endl;
    }
}

//checks if the airport has already been visited.
bool visited(string airport)
{
    stack<string> copy_ = copyStack(path);
    while (!copy_.empty()) {
        if (copy_.top() == airport)
            return true;
        copy_.pop();
    }
    return false;
}

//checks if the path has reached its destination
bool done(string end)
{
    if (path.empty()) {
        return false;
    } else if (path.top() == end) {
        return true;
    }
    return false;
}

//resets the path stack by popping everything
void popAll()
{
    while (!path.empty()) {
        path.pop();
    }
}

//IDDFS algorithm 
void IDDFS(string start, string sss, int DB, Graph graf)
{
    if (depthbound < DB)
        return;
    unordered_map<string, Graph::Node*> &dests = graf.pts_[start]->children;
    for (auto it = dests.begin(); it != dests.end(); ++it) {
        DB = path.size();
        if (!visited((*it).first)) {
            path.push((*it).first);
            if ((*it).first.find(sss) != string::npos) {
                savePath(path);
                path.pop();
            } else if (DB < depthbound) {
                IDDFS((*it).first, sss, ++DB, graf);
                path.pop();
            } else {
                path.pop();
            }
        }
    }
    return;
}

//Checks the total distance of every saved path. returns the shortest path in miles
double distance(Graph gg){
  string one, two;
  double distt =0;
  double best = 0;
  for(unsigned i=0; i<saved.size(); i++){
    stack<string> temp = copyStack(saved[i]);
    distt = 0;
    while(temp.size() > 1){
      one = temp.top();
      temp.pop();
      two = temp.top();
      pair<double, double> onee = gg.locations_[one];
      pair<double, double> twoo = gg.locations_[two]; 
      double t = dist(onee.first, onee.second, twoo.first, twoo.second);
      distt += t;
    }
    if(distt < best || best ==0){
      best = distt;
      best_i = i;
    }
  }
  return best;
}

//Manager function calls all the different functions and outputs relevant data
void manager(string st, string en, Graph graf, int num)
{
    while (depthbound < num+1) {
        popAll();
        path.push(st);
        depthbound++;
        IDDFS(st, en, 1, graf);
    }
    printSaved(st);
    double z = distance(graf);
    cout << "Found " << counter2 <<" paths.\nShortest path is ";
    while(!saved[best_i].empty()){
      if(saved[best_i].top() == st)
        cout << saved[best_i].top();
      else
        cout << saved[best_i].top() << " <-- ";
      saved[best_i].pop();
    }
    cout << " with a distance of " << z << " miles.\n";
}
