#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <stack>

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
  for(int i=0; i<saved.size(); i++){
    stack<string> comp = copyStack(saved[i]);
    stack<string> temp = copyStack(saveThis);
    if(isEqual(temp, comp)){
      cout <<endl;
      return;
    }
  }
  counter2++;
  stack<string> save_ = copyStack(saveThis);
  saved.push_back(save_);
  cout << " < New unique path.\n";
}

void printSaved(){
  for(int i=0; i<saved.size(); i++){
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
  /*stack<string> copy2;
  cout << endl << "Possible path: \n";
  while(!path.empty()){
    cout << path.top() << endl;
    copy2.push(path.top());
    path.pop();
  }
  while(!copy2.empty()){
    path.push(copy2.top());
    copy2.pop();
  }
  counter++;
  cout << endl;*/
  
  stack<string> copy_ = copyStack(path);
  cout << "Found path with " << copy_.size()-2  << " stops: ";
  while(!copy_.empty()){
    cout << copy_.top() << " <-- ";
    copy_.pop();
  }
  counter++;
}

bool visited(string airport){
  /*bool ret = false;
  stack<string> copy = copyStack(path);
  while(!path.empty()){
    if(path.top() == airport)
      ret = true;
    copy.push(path.top());
    path.pop();
  }
  while(!copy.empty()){
    path.push(copy.top());
    copy.pop();
  }
  return ret;*/
  
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

void findpath(string start, string end, int DB){
  if(depthbound < DB)
    return;
  string line, word;
  int stops;
  vector<string> row;
  ifstream myfile ("routes.dat");
  if(myfile.is_open()){
    while(getline(myfile,line)){
      DB = path.size();
      stringstream s(line);
      row.clear();
      while(getline(s, word, ',')){
        row.push_back(word);
      }
      if(row[2].find(start) != string::npos && !visited(row[4])){
        stringstream ss(row[7]); 
        ss >> stops;
        path.push(row[4]);
        //cout << "pushing " << row[4] << " from " << row[2] << "   DB: " << DB <<endl;
        if(stops == 0 && row[4]== end){
          output();
          savePath(path);
          path.pop();
          //return;
        }
        else if(DB < depthbound){
          //cout << "DB: " << DB << " depth: " << depthbound << endl;
          findpath(row[4], end, ++DB);
          //if(path.top() == end)
            //return;
          path.pop();
        }
        else{
            path.pop();
        }
      }
    }
    myfile.close();
  } else cout << "Unable to open file";
  return;
}

void popAll(){
  while(!path.empty()){
    path.pop();
  }
}

int main()
{
  string start = "BOS";
  string end = "ICN";
  while(/*!done(end) &&*/ depthbound < 3){
    popAll();
    path.push(start);
    depthbound++;
    cout << "depthbound: " << depthbound <<endl;
    findpath(start, end, 1);
  }
  cout << "*** PRINTING SAVED PATHS ***\n\n";
  printSaved();
  cout << "Found " << counter <<" paths.\n";
  cout << counter2 << " are unique.\n";
  
  return 0;
}
