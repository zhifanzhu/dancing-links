#include <iostream>
#include <vector>
#include <sstream>

#include "../dlx.h"

int main() {
    vector<string> items;
    vector<set<string>> options;
    // items = {"a", "b", "c", "d", "e", "f", "g"};
    // options = {
    //     {"c", "e"}, 
    //     {"a", "d", "g"}, 
    //     {"b", "c", "f"}, 
    //     {"a", "d", "f"}, 
    //     {"b", "g"}, 
    //     {"d", "e", "g"},
    // };

    string buf;
    std::getline(std::cin, buf);
    std::istringstream iss(buf);
    std::string token;
    while (iss >> token) {
        items.push_back(token);
    }
    while (std::getline(std::cin, buf)) {
        std::istringstream iss(buf);
        std::string token;
        set<string> option;
        while (iss >> token) {
            option.insert(token);
        }
        options.push_back(option);
    }

    auto dlx = DLX<string>(true);
    dlx.solve(items, options);
    // dlx.print_memory_table(std::cout);
    // cout << "Exucution DONE!" << endl;
    dlx.print_solution();

    return 0;
}
