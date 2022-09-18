//#include <fstream>
#include <iostream>
#include <string>
//#include <sstream>
#include <vector>
//#include <math.h>
#include <stack>
#include "Graph.h" 

using namespace std;

int counter = 0;
int counter2 =0;
stack<string> path;
int depthbound = 0;
vector<stack<string> > saved;

stack<string> copyStack(stack<string> copyThis){
  stack<string> copy_;
  stack<string> temp;
  while(!copyThis.empty()){
    temp.push(copyThis.top());
    copyThis.pop();
  }
  while(!temp.empty()){
    copyThis.push(temp.top());
    copy_.push(temp.top());
    temp.pop();
  }
  return copy_;
}

bool isEqual(stack<string> a, stack<string> b){
  stack<string> copyA = copyStack(a);
  stack<string> copyB = copyStack(b);
  
  if(copyA.size() != copyB.size())
    return false;
  while(!copyA.empty()){
    if(copyA.top() != copyB.top())
      return false;
    copyA.pop();
    copyB.pop();
  }
  return true;
}

void savePath(stack<string> saveThis){
  for(unsigned i=0; i<saved.size(); i++){
    stack<string> comp = copyStack(saved[i]);
    stack<string> temp = copyStack(saveThis);
    if(isEqual(temp, comp)){
      cout <<endl;
      cout << "Found this one before\n";
      return;
    }
  }
  counter2++;
  stack<string> save_ = copyStack(saveThis);
  saved.push_back(save_);
}

void printSaved(){
  for(unsigned i=0; i<saved.size(); i++){
    stack<string> temp = copyStack(saved[i]);
    cout << "Possible path with " << temp.size()-2 << " stops: ";
    while(!temp.empty()){
      cout << temp.top() << " <-- ";
      temp.pop();
    }
    cout << endl;
  }
}

void output(){
  stack<string> copy_ = copyStack(path);
  cout << "Found path with " << copy_.size()-2  << " stops: \n";
  while(!copy_.empty()){
    cout << copy_.top() << " <-- ";
    copy_.pop();
  }
  counter++;
}

bool visited(string airport){
  stack<string> copy_ = copyStack(path);
  while(!copy_.empty()){
    if(copy_.top() == airport)
      return true;
    copy_.pop();
  }
  return false;
}

bool done(string end){
  if(path.empty())
    return false;
  else if(path.top() == end)
    return true;
  return false;
}

void popAll(){
  while(!path.empty()){
    path.pop();
  }
}

void IDDFS(string start, string sss, int DB, Graph graf){
  if(depthbound < DB)
    return;
  unordered_map<string, Graph::Node*> &dests = graf.getDestinations(start);
  for(auto it = dests.begin(); it != dests.end(); ++it){
    DB = path.size();
    if(!visited((*it).first)){
      path.push((*it).first);
      //cout << "pushing " << (*it).first << " from " << start << "   DB: " << DB <<endl;
      if((*it).first.find(sss) != string::npos){
        output();
        savePath(path);
        path.pop();
      }
      else if(DB < depthbound){
        IDDFS((*it).first, sss, ++DB, graf);
        path.pop();
      }
      else{
        path.pop();
      }
    }
  }
  return;
}

void manager(string st, string en, Graph graf, int num){
  while(/*!done(end) &&*/ depthbound < num+1){
    popAll();
    path.push(st);
    depthbound++;
    cout << "depthbound: " << depthbound <<endl;
    IDDFS(st, en, 1, graf);
  }
  cout << "*** PRINTING SAVED PATHS ***\n\n";
  printSaved();
  cout << "Found " << counter <<" paths.\n";
  cout << counter2 << " are unique.\n";
  
}
