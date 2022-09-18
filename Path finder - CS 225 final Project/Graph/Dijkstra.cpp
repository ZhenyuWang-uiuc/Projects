/*
 * Author: Zhenyu Wang
 * Date: 5/10/2021
 */

/**
 * PROGRESS:
 *    v1: old version of Dijkstra Algorithm
 *    reason: I find it is hard to understand the pseudocode from class page.
 *
 *    v2: new version of Dijkstra Algorithm
 *    reason: I think this one is much reasonable and clear.
 *
 *    v3: final version of Dijkstra Algorithm
 *    reason: After few hours of thinking. I reference both of them becasue both of them are useful.
 *
 *    ex: I also referenced the following links to try to compare specialized std::pair 
 *        Because I want to use heap STL to create a min heap.
 *        Step 1: define a free operator > in the same namespace with std::pair
 *        Step 2: #include <queue> --> for using heap STL
 *                #include <functional>  --> for using std::greater
 *        Step 3: Basing on http://www.cplusplus.com/reference/queue/priority_queue/, try to create a min heap
 *
 * LINKS:
 *     pseudocode
 *        - http://www.gitta.info/Accessibiliti/en/html/Dijkstra_learningObject1.html
 *        - https://courses.engr.illinois.edu/cs225/sp2021/resources/dijkstra/
 *     links for comparing specialized std::pair
 *        - https://stackoverflow.com/questions/9526430/c-overloading-operator-for-stdpair
 *        - https://stackoverflow.com/questions/9849301/pair-equal-operator-overloading-for-inserting-into-set
 *        - https://stackoverflow.com/questions/4652932/why-define-operator-or-outside-a-class-and-how-to-do-it-properly
 */

#include <string>
#include <vector>
#include <limits>
#include <queue>
#include <functional>
#include <unordered_map>
#include <unordered_set>

using namespace std;

typedef pair<string, double> p;

namespace std {

    bool operator >(const p & f, const p & s)
    {
        return f.second > s.second;
    }

}

/**
 * Above code also can be replaced by the following code
 *
 * struct p {
 *     string first;
 *     double second;
 *     p(string f, double s) : first(f), second(s) {}
 *     bool operator >(const p & s) const { // make sure add "const" modifier at the end of function
 *         return this->second > s.second;
 *     }
 * };
 */

pair<vector<string>, double> Dijkstra(Graph graph)
{
    double initialDist = std::numeric_limits<double>::max();
    unordered_map<string, double> distance;
    unordered_map<string, string> previous;
    priority_queue<p, vector<p>, greater<p>> q;
    unordered_set<string> visited;

    for (auto vertex : graph.locations_) {
        distance[vertex.first] = initialDist;
        previous[vertex.first] = string();
    }

    distance[graph.start_] = 0;
    q.push(p(graph.start_, 0));

    while (q.top().first != graph.end_) {
        p cur = q.top();
        q.pop();
        for (auto it = graph.pts_[cur.first]->children.begin(); it != graph.pts_[cur.first]->children.end(); ++it) {
            if (visited.find((*it).first) == visited.end()) {
                pair<double, double> cur_l = graph.locations_[cur.first];
                pair<double, double> chd_l = graph.locations_[(*it).first]; 
                double d = graph.calcDist_(cur_l.first, cur_l.second, chd_l.first, chd_l.second);
                double tmpDist = distance[cur.first] + d;
                if (tmpDist < distance[(*it).first]) {
                    distance[(*it).first] = tmpDist;
                    previous[(*it).first] = cur.first;
                    q.push(p((*it).first, tmpDist));
                }
            }
        }
        visited.insert(cur.first);
    }

    vector<string> ans;
    string next = q.top().first;
    while (next != graph.start_) {
        ans.push_back(next);
        next = previous[next];
    }
    ans.push_back(next);
    return pair<vector<string>, double>(ans, distance[graph.end_]);
}
