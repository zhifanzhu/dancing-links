#pragma once

#include <iostream>
#include <strstream>
#include <vector>
#include <string>
#include <set>
#include <map>

#define DBG(x) cerr << #x << " = " << (x) << endl;

using namespace std;

template <typename T>
struct ItemNodeType {
    T NAME;
    ItemNodeType<T> *LLINK;
    ItemNodeType<T> *RLINK;
    int nid; // node_id
};
struct Node {
    int LEN; // or top
    Node *ULINK;
    Node *DLINK;
    int nid; // node_id
    int TOP() const { return LEN; }
};

template <typename T>
class DLX {
    using ItemNode = ItemNodeType<T>;
    unordered_map<int, ItemNode*> i_nodes; // header, <readable-addr, ptr>
    unordered_map<int, Node*> x_nodes; // content, <readable-addr, ptr>

    int _l;
    // vector<int> _x;
    unordered_map<int, int> _x;
    vector<int> _solution;
    size_t _num_solutions;
    bool _terminate;

    // solver settings
    bool store_solutions;

public:

    DLX() {
        _l = 0;
        _num_solutions = 0;
        _terminate = false;
        store_solutions = false;
    }
    DLX(bool store_solutions) {
        _l = 0;
        _num_solutions = 0;
        _terminate = false;
        this->store_solutions = store_solutions;
    }

    void setup_items(vector<T> &items) {
        i_nodes[0] = new ItemNode{T(), nullptr, nullptr, 0};
        x_nodes[0] = new Node{INT_MAX, nullptr, nullptr, 0};
        ItemNode *cur = i_nodes[0];
        for (int i = 1; i <= items.size(); ++i) {
            i_nodes[i] = new ItemNode{items[i-1], nullptr, nullptr, i};
            cur->RLINK = i_nodes[i];
            i_nodes[i]->LLINK = cur;
            cur = i_nodes[i];

            x_nodes[i] = new Node{0, nullptr, nullptr, i};
            x_nodes[i]->ULINK = x_nodes[i];
            x_nodes[i]->DLINK = x_nodes[i];
        }
        cur->RLINK = i_nodes[0];
        i_nodes[0]->LLINK = cur;
    }

    void setup_options(vector<set<T>> &options) {
        int x = x_nodes.size();
        map<T, int> tops;
        for (auto pr : i_nodes) {
            tops[pr.second->NAME] = pr.first;
        }
        map<T, Node*> prev_ups;
        for (auto pr : i_nodes) {
            prev_ups[pr.second->NAME] = x_nodes[pr.first];
        }

        Node *wrap_node = new Node{0, nullptr, nullptr, x};
        x_nodes[x] = wrap_node;
        x++;
        for (int option_id = 0; option_id < options.size(); ++option_id) {
            set<T> & option = options[option_id];

            int first_x = x;

            for (auto & choice : option) {
                Node *top_node = x_nodes[tops[choice]];
                Node *prev_up = prev_ups[choice];
                x_nodes[x] = new Node{top_node->nid, prev_up, prev_up->DLINK, x};
                prev_up->DLINK->ULINK = x_nodes[x];
                prev_up->DLINK = x_nodes[x];
                prev_ups[choice] = x_nodes[x];
                top_node->LEN++;
                x++;
            }

            wrap_node->DLINK = x_nodes[x-1];
            wrap_node = new Node{-option_id-1, x_nodes[first_x], nullptr, x};
            x_nodes[x] = wrap_node;
            x++;
        }
    }

    void setup_memory(vector<T> &items, vector<set<T>> &options) {
        setup_items(items);
        setup_options(options);
    }

    void print_memory_table(ostream& os) const {
        int num_x_rows = (x_nodes.size() + i_nodes.size() - 1) / i_nodes.size();
        int num_lines = 4 * (1 + num_x_rows); 
        vector<strstream> lines(num_lines);
        lines[0] << "i";
        lines[1] << "NAME(i)";
        lines[2] << "LLINK(i)";
        lines[3] << "RLINK(i)";
        lines[4] << "x";
        lines[5] << "LEN(x)";
        lines[6] << "ULINK(x)";
        lines[7] << "DLINK(x)";
        for (int i = 0; i < num_x_rows-1; ++i) {
            lines[4*(i+2)+0] << "x";
            lines[4*(i+2)+1] << "TOP(x)";
            lines[4*(i+2)+2] << "ULINK(x)";
            lines[4*(i+2)+3] << "DLINK(x)";
        }

        for (int i = 0; i < i_nodes.size(); i++) {
            ItemNode *inode = i_nodes.at(i);
            lines[0] << "," << i;
            lines[1] << "," << inode->NAME;
            lines[2] << "," << inode->LLINK->nid;
            lines[3] << "," << inode->RLINK->nid;

            Node *xnode = x_nodes.at(i);
            if (i == 0) {
                lines[4] << ",0";
                lines[5] << ",_";
                lines[6] << ",_";
                lines[7] << ",_";
            } else {
                lines[4] << "," << i;
                lines[5] << "," << xnode->LEN;
                lines[6] << "," << xnode->ULINK->nid;
                lines[7] << "," << xnode->DLINK->nid;
            }
        }

        for (int x = i_nodes.size(); x < x_nodes.size(); ++x) {
            int row_id = x / (i_nodes.size()) + 1;
            lines[row_id*4+0] << "," << x;
            lines[row_id*4+1] << "," << x_nodes.at(x)->LEN;
            string ulink_id = x_nodes.at(x)->ULINK == nullptr ? "_" : to_string(x_nodes.at(x)->ULINK->nid);
            string dlink_id = x_nodes.at(x)->DLINK == nullptr ? "_" : to_string(x_nodes.at(x)->DLINK->nid);
            lines[row_id*4+2] << "," << ulink_id;
            lines[row_id*4+3] << "," << dlink_id;
        }

        for (auto &line : lines)
            os << line.str() << std::endl;
    }

    /* Low-level operations */
    // Cover many rows that has item
    void cover(int item_id) {
        int p = x_nodes[item_id]->DLINK->nid;
        for (; p != item_id; ) {
            hide(p);
            p = x_nodes[p]->DLINK->nid;
        }
        ItemNode *l = i_nodes[item_id]->LLINK;
        ItemNode *r = i_nodes[item_id]->RLINK;
        l->RLINK = r;
        r->LLINK = l;
    }

    // p is readable-address of x-node
    // Hide single row that has p
    void hide(int p) {
        for (int q = p + 1; q != p; ) {
            int x = x_nodes[q]->TOP();
            Node* u = x_nodes[q]->ULINK;
            Node* d = x_nodes[q]->DLINK;
            if (x <= 0) {
                q = u->nid;
            } else {
                u->DLINK = d;
                d->ULINK = u;
                x_nodes[x]->LEN--;
                q++;
            }
        }
    }

    void uncover(int item_id) {
        ItemNode *l = i_nodes[item_id]->LLINK;
        ItemNode *r = i_nodes[item_id]->RLINK;
        l->RLINK = i_nodes[item_id];
        r->LLINK = i_nodes[item_id];
        int p = x_nodes[item_id]->ULINK->nid;
        for (; p != item_id; ) {
            unhide(p);
            p = x_nodes[p]->ULINK->nid;
        }
    }

    void unhide(int p) {
        for (int q = p - 1; q != p; ) {
            int x = x_nodes[q]->TOP();
            Node* u = x_nodes[q]->ULINK;
            Node* d = x_nodes[q]->DLINK;
            if (x <= 0) {
                q = d->nid;
            } else {
                u->DLINK = d->ULINK = x_nodes[q];
                x_nodes[x]->LEN++;
                q--;
            }
        }
    }

    void debug_state() {
        printf("l = %d, x[l] = %d\n", _l, _x[_l]);
        for (int t = 0, p = 0; !(t != 0 && p == 0); p = i_nodes[p]->RLINK->nid, ++t) {
            printf("i = %d, LLINK(i) = %d, RLINK(i) = %d\n", p, i_nodes[p]->LLINK->nid, i_nodes[p]->RLINK->nid);
            if (p == 0) continue;

            for (int t2 = 0, q = p; !(t2 != 0 && q == p); q = x_nodes[q]->DLINK->nid, ++t2) {
                printf("x = %d, ULINK(x) = %d, DLINK(x) = %d\n", q, x_nodes[q]->ULINK->nid, x_nodes[q]->DLINK->nid);
            }
        }

        cerr << "Exiting...\n";
        exit(-1);
    }

    void X2() {
        if (i_nodes[0]->RLINK->nid == 0) {
            _num_solutions++;
            if (this->store_solutions) {
                for (int i = 0; i < _l; ++i) {
                    _solution.push_back(_x[i]);
                }
            }
            X8();
            return;
        }
        /* X3 */
        int item_id = i_nodes[0]->RLINK->nid;
        /* X4 */
        cover(item_id);
        _x[_l] = x_nodes[item_id]->DLINK->nid;
        // printf("X2: x[%d] <- %d\n", _l, _x[_l]);

        /* X5: Cover all affected columns */
        if (_x[_l] == item_id) {
            X7(item_id);
        } else {
            int p = _x[_l] + 1;
            for (; p != _x[_l]; ) {
                int j = x_nodes[p]->TOP();
                if (j <= 0)
                    p = x_nodes[p]->ULINK->nid;
                else {
                    cover(j);
                    p++;
                }
            }
            _l++;
            X2();
        }
    }
    void X6() {
        /* X6: Reverse of X5 */
        int p = _x[_l] - 1;
        for (; p != _x[_l]; ) {
            int j = x_nodes[p]->TOP();
            if (j <= 0)
                p = x_nodes[p]->DLINK->nid;
            else {
                uncover(j);
                p--;
            }
        }
        int item_id = x_nodes[_x[_l]]->TOP();
        _x[_l] = x_nodes[_x[_l]]->DLINK->nid;
        // printf("X6: x[%d] <- %d\n", _l, _x[_l]);

        /* X5: Cover all affected columns */
        if (_x[_l] == item_id) {
            X7(item_id);
        } else {
            int p = _x[_l] + 1;
            for (; p != _x[_l]; ) {
                int j = x_nodes[p]->TOP();
                if (j <= 0)
                    p = x_nodes[p]->ULINK->nid;
                else {
                    cover(j);
                    p++;
                }
            }
            _l++;
            X2();
        }

    }
    void X7(int item_id) {
        // printf("Uncover %d\n", item_id);
        uncover(item_id);
        X8();
    }
    void X8() {
        if (_l == 0) {
            _terminate = true;
            return;
        }
        _l--;
        X6();
    }

    void X_goto() {
        // use goto to avoid stack overflow
        int p;
        int item_id;
    X2:
        if (i_nodes[0]->RLINK->nid == 0) {
            _num_solutions++;
            if (this->store_solutions) {
                for (int i = 0; i < _l; ++i) {
                    _solution.push_back(_x[i]);
                }
            }
            goto X8;
        }
    X3:
        item_id = i_nodes[0]->RLINK->nid;
    X4:
        cover(item_id);
        _x[_l] = x_nodes[item_id]->DLINK->nid;
    X5:
        if (_x[_l] == item_id) {
            goto X7;
        } else {
            p = _x[_l] + 1;
            for (; p != _x[_l]; ) {
                int j = x_nodes[p]->TOP();
                if (j <= 0)
                    p = x_nodes[p]->ULINK->nid;
                else {
                    cover(j);
                    p++;
                }
            }
            _l++;
            goto X2;
        }
    X6:
        p = _x[_l] - 1;
        for (; p != _x[_l]; ) {
            int j = x_nodes[p]->TOP();
            if (j <= 0)
                p = x_nodes[p]->DLINK->nid;
            else {
                uncover(j);
                p--;
            }
        }
        item_id = x_nodes[_x[_l]]->TOP();
        _x[_l] = x_nodes[_x[_l]]->DLINK->nid;
        goto X5;
    X7:
        uncover(item_id);
    X8:
        if (_l == 0) {
            _terminate = true;
            return;
        }
        _l--;
        goto X6;
    }

    void solve(vector<T> &items, vector<set<T>> &options) {
        /* X1 */
        if (items.size() == 0) {
            cout << "Empty Items" << endl;
            return;
        }
        setup_memory(items, options);
        _l = 0;
        // DBG(x_nodes.size());
        // X2();
        X_goto();
    }

    size_t num_solutions() const { return _num_solutions; }

    void print_solution() {
        // cout << "Solution: \n";
        for (int i = 0; i < _solution.size(); ++i) {
            cout << i_nodes[x_nodes[_solution[i]]->TOP()]->NAME << " ";
            for (int p = _solution[i] + 1; p != _solution[i]; ) {
                int j = x_nodes[p]->TOP();
                if (j <= 0)
                    p = x_nodes[p]->ULINK->nid;
                else {
                    cout << i_nodes[j]->NAME << " ";
                    p++;
                }
            }
            cout << endl;
        }
    }

};
