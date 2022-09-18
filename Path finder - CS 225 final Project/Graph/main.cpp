#include "Graph.h"
#include "IDDFS.cpp"
#include "Dijkstra.cpp"
#include <algorithm>

using namespace std;    

void print(vector<string> & ans) {
    for (unsigned i = 0; i < ans.size(); ++i) {
        if (i == ans.size() - 1)
            cout << ans[i] << endl;
        else
            cout << ans[i] << " <-- ";
    }
    cout << endl;
}

int main(int argc, const char** argv) {
    cout << endl;
    if (argc != 2) {  // tell users how to use the command tool
        cout << "Sorry, you input invalid number of arguements\n";
        cout << "You can input - \"DFS\" to view the DFS solution\n";
        cout << "              - \"IDDFS\" to view the IDDFS solution\n";
        cout << "              - \"DIJ\" to view the Dijkstra solution\n";
        cout << "              - \"all\" to view all solutions\n";
        cout << "Usage: ./Graph <DFS> <IDDFS> <DIJ> <all>\n\n";
        return 0;
    } else {  // check whether the command is valid
        bool isValid = false;
        char commands[][6] = {"all", "DFS", "IDDFS", "DIJ"};
        for (int i = 0; i < 4; ++i) {
            if (strcmp(argv[1], commands[i]) == 0) {
                isValid = true;
                break;
            }
        }
        if (!isValid) {
            cout << "Invalid command. Please try again...\n\n";
            return 0;
        }
    }

    string source, dest, flag;
    int stops;

    cout << "From airport: "; cin >> source;
    cout << "..To airport: "; cin >> dest;
    cout << endl;

    // create a graph
    Graph g = Graph("routes.dat", "airports.dat", pair<string, string>(source, dest));
    flag = g.createGraph();

    // check the feasibility of the graph 
    if (flag != "success") {
        cout << "Sorry, there is a problem occurs during the creation.\n";
        cout << "Here is the error message:\n";
        cout << flag << "\n\n";
        return 0;
    } else if (!g.isReachable()) {
        cout << "Sorry, the starting point and end point are disconnected. Please try again..\n\n";
        return 0;
    } 

    if (strcmp(argv[1], "all") == 0)
        cout << "Here are all of our solutions\n\n";

    if (strcmp(argv[1], "IDDFS") == 0 || strcmp(argv[1], "all") == 0) {
        cout << "Here's the IDDFS solution\n";
        cout << "note: 2 stops has ~9 minute runtime. 3 exceeds CPU runtime.\n";
        cout << "Max num of stops: "; cin >> stops;
        cout << "Finding EVERY path from " << source << " to " << dest <<
                " with no more than " << stops << " intermediate stops...\n";
        manager(source, dest, g, stops);
        cout << endl;
    }
 
    if (strcmp(argv[1], "DFS") == 0 || strcmp(argv[1], "all") == 0) {
        vector<string> ans = g.DFS();
        reverse(ans.begin(), ans.end());
        cout << "Here's the DFS solution\n";
        cout << ans.size() << endl;
        print(ans);
    }
 
    if (strcmp(argv[1], "DIJ") == 0 || strcmp(argv[1], "all") == 0) {
        g.AddCalcDistMethod(dist);
        pair<vector<string>, double> ans = Dijkstra(g);
        cout << "Here's the Dijkstra solution with total distance " << ans.second << " miles\n";
        print(ans.first);
    }

    return 0;
}
