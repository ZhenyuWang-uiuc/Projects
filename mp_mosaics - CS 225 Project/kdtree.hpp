/**
 * @file kdtree.cpp
 * Implementation of KDTree class.
 */

#include <utility>
#include <algorithm>

using namespace std;

template <int Dim>
bool KDTree<Dim>::smallerDimVal(const Point<Dim>& first,
                                const Point<Dim>& second, int curDim) const
{
    if (curDim >= 0) {
        if (first[curDim] == second[curDim]) { 
            return first < second;
        }
        return first[curDim] < second[curDim];
    }

    return false;
}

template <int Dim>
bool KDTree<Dim>::shouldReplace(const Point<Dim>& target,
                                const Point<Dim>& currentBest,
                                const Point<Dim>& potential) const
{
    double c = 0;
    double p = 0;
    for (int i = 0; i < Dim; i++) {
        c += (target[i] - currentBest[i]) * (target[i] - currentBest[i]);
        p += (target[i] - potential[i]) * (target[i] - potential[i]);
    }   
    if (c == p) {
        return potential < currentBest;
    }
    return p < c;
}

template <int Dim>
KDTree<Dim>::KDTree(const vector<Point<Dim>>& newPoints)
{
    if (newPoints.size() >= 1) {
        vector<Point<Dim>> pts = newPoints;
        size = pts.size();
         _sort(pts, 0);
        root = new KDTreeNode(pts[(pts.size() - 1) / 2]);
        if (Dim == 1) { _createTree(root->left, root->right, pts, 0); }
        else { _createTree(root->left, root->right, pts, 1); }
    } else {
        size = 0;
        root = nullptr;
    }
}

template <int Dim>
KDTree<Dim>::KDTree(const KDTree<Dim>& other) {
    *this = other;
}

template <int Dim>
const KDTree<Dim>& KDTree<Dim>::operator=(const KDTree<Dim>& rhs) {
    if (*this != rhs) {
        size = rhs.size;
        _free(root);
        _copy(rhs.root, root);
    }

    return *this;
}

template <int Dim>
KDTree<Dim>::~KDTree() {
    _free(root);
}

template <int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighbor(const Point<Dim>& query) const
{
    if (root == nullptr) {
        return Point<Dim>();
    } else {
        Point<Dim> answer;
        bool flag = false;
        double distance = 0;
        _findNearestNeighborHelper(query, root, answer, 0, flag, distance);
        return answer;
    }
}

template <int Dim>
void KDTree<Dim>::_sort(vector<Point<Dim>>& newPoints, int d)
{
    if (newPoints.size() > 1) {
       for (unsigned i = 0; i < newPoints.size() - 1; i++) {
           for (unsigned j = 0; j < newPoints.size() - i - 1; j++) {
               if (!smallerDimVal(newPoints[j], newPoints[j + 1], d)) {
                   Point<Dim> tmp = newPoints[j + 1];
                   newPoints[j + 1] = newPoints[j];
                   newPoints[j] = tmp;
               }
           }
       }
    }
}

/**
 * Notice: node with the same value will insert at the right. 
 *         when the size of the vector is even, the median point 
 *         is left-leaning. 
 *         for example vector = {3, 3}, the meidan is first 3
 *         in the next recurive call, left = {} and right = {3};
 *         so the node with same value will insert at the right
 */
template <int Dim>
void KDTree<Dim>::_createTree(KDTreeNode *& l, KDTreeNode *& r, vector<Point<Dim>>& pts, int d)
{
    if (pts.size() > 1) {
        vector<Point<Dim>> left = vector<Point<Dim>>(pts.begin(), pts.begin() + (int)((pts.size() - 1) / 2));
        vector<Point<Dim>> right = vector<Point<Dim>>(pts.begin() + (int)((pts.size() - 1) / 2) + 1, pts.end());
        _sort(left, d);
        _sort(right, d);
        l = (left.size() > 0) ? new KDTreeNode(left[(left.size() - 1) / 2]) : nullptr;
        r = new KDTreeNode(right[(right.size() - 1) / 2]);
        d = (d == Dim - 1) ? 0 : d + 1;
        _createTree(l->left, l->right, left, d);
        _createTree(r->left, r->right, right, d);
    }
}

template <int Dim>
void KDTree<Dim>::_free(KDTreeNode * cur)
{
    if (cur != nullptr) {
        if (cur->left != nullptr) {
            _free(cur->left);
        }

        if (cur->right != nullptr) {
            _free(cur->right);
        }

        delete cur;
    }
}

template <int Dim>
void KDTree<Dim>::_copy(KDTreeNode *& cur, KDTreeNode *& r)
{
    if (cur != nullptr) {
        r = new KDTreeNode(cur->point);
        _copy(cur->right, r->right);
        _copy(cur->left, r->left);
    } 

}

template <int Dim>
void KDTree<Dim>::_findNearestNeighborHelper(const Point<Dim>& query, KDTreeNode * cur, 
                                                   Point<Dim> & smallest, int d, 
                                                   bool & flag, double & distance) const
{
    if (cur->left == nullptr && cur->right == nullptr && !flag) {
        smallest = cur->point;
        flag = true;
        distance = getDistanceSqrt(query, smallest);
        return;
    } else if (cur->left == nullptr && cur->right == nullptr && flag) {
        if (shouldReplace(query, smallest, cur->point)) {
            smallest = cur->point;
            distance = getDistanceSqrt(query, smallest);
        }
        return;
    }

    // following code will try to find the smallest box containing query
    int dimension = (d == Dim - 1) ? 0 : d + 1;
    // Notice: CHECK LEFT! If left is null, crush the program. 
    //         Even I do the action to protect program, program still can update the smallest before I reach the first leaf node,
    //         which means I may lose the current smallest!
    if (cur->point[d] > query[d] && cur->left != nullptr) {
        _findNearestNeighborHelper(query, cur->left, smallest, dimension, flag, distance);
        if (shouldReplace(query, smallest, cur->point)) {
            smallest = cur->point;
            distance = getDistanceSqrt(query, smallest);
         }
         if (distance == 0) { return; }

         if ((cur->point[d] - query[d]) * (cur->point[d] - query[d]) <= distance) {
            _findNearestNeighborHelper(query, cur->right, smallest, dimension, flag, distance);
         }
    } else {
        _findNearestNeighborHelper(query, cur->right, smallest, dimension, flag, distance);
        if (shouldReplace(query, smallest, cur->point)) {
            smallest = cur->point;
            distance = getDistanceSqrt(query, smallest);
        }
        if (distance == 0) { return; }
       
        if ((cur->point[d] - query[d]) * (cur->point[d] - query[d]) <= distance && cur->left != nullptr) {
            _findNearestNeighborHelper(query, cur->left, smallest, dimension, flag, distance);
        }
   }
}

template <int Dim>
double KDTree<Dim>::getDistanceSqrt(const Point<Dim>& target, const Point<Dim>& cur) const
{
    double d = 0;
    for (int i = 0; i < Dim; i++) { d += (target[i] - cur[i]) * (target[i] - cur[i]); }
    return d;
}
