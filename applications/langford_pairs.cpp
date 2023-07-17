#include <iostream>
#include <set>
#include "../dlx.h"

using namespace std;

long long langford_pairs(int n) {
    // e.g. (1, s1, s3) as (-1, 1, 3)
    vector<int> items;
    vector<set<int>> options;
    for (int i = 1; i <= n; ++i) {
        items.push_back(-i);
        items.push_back(2*i-1);
        items.push_back(2*i);
    }
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j+i+1 <= 2*n; ++j) {
            set<int> option;
            option.insert(-i);
            option.insert(j);
            option.insert(j+i+1);
            options.push_back(option);
        }
    }

    auto dlx = DLX<int>();
    dlx.solve(items, options);
    // dlx.print_solution();
    long long res = dlx.num_solutions();
    return res;
}

int main(int argc, char **argv) {
    /*
    example: 
    n=3, solution = 2, 3, 1, 2, 1, 3.
    
    n,sol
    1,0
    2,0
    3,1
    4,1
    5,0
    6,0
    7,26
    8,150,
    9,0
    10,0
    11,17792
    12,108114
    ...
    16,326,721,800
    */
    int n;
    n = atoi(argv[1]);
    cout << langford_pairs(n) << endl;
    return 0;
}