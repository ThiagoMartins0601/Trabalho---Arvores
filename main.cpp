#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <random>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <memory>

struct TrieNode {
    std::unordered_map<char, TrieNode*> children;
    bool is_end = false;
    std::string value;
};

class AdvancedTrie {
private:
    TrieNode* root;

public:
    AdvancedTrie() { root = new TrieNode(); }

    void insert(const std::string& word, const std::string& val) {
        TrieNode* curr = root;
        for (char c : word) {
            if (!curr->children.count(c)) {
                curr->children[c] = new TrieNode();
            }
            curr = curr->children[c];
        }
        curr->is_end = true;
        curr->value = val;
    }

    bool search(const std::string& word, std::string& out_val) const {
        TrieNode* curr = root;
        for (char c : word) {
            if (!curr->children.count(c)) return false;
            curr = curr->children[c];
        }
        if (curr->is_end) {
            out_val = curr->value;
            return true;
        }
        return false;
    }
};

struct PatriciaNode {
    std::unordered_map<char, std::pair<std::string, PatriciaNode*>> edges;
    bool is_leaf = false;
    int payload = -1;
};

class PatriciaTree {
private:
    PatriciaNode* root;

public:
    PatriciaTree() { root = new PatriciaNode(); }

    void insert(const std::string& key, int data) {
        PatriciaNode* curr = root;
        size_t idx = 0;
        while (idx < key.size()) {
            char f_char = key[idx];
            if (!curr->edges.count(f_char)) {
                curr->edges[f_char] = {key.substr(idx), new PatriciaNode(true, data)};
                return;
            }
            auto& edge = curr->edges[f_char];
            std::string& edge_str = edge.first;
            PatriciaNode* next_node = edge.second;
            size_t match = 0;
            while (match < edge_str.size() && (idx + match) < key.size() && edge_str[match] == key[idx + match]) {
                match++;
            }
            if (match == edge_str.size()) {
                idx += match;
                curr = next_node;
            } else {
                std::string common = edge_str.substr(0, match);
                std::string remaining_old = edge_str.substr(match);
                std::string remaining_new = key.substr(idx + match);
                PatriciaNode* mid_node = new PatriciaNode();
                mid_node->edges[remaining_old[0]] = {remaining_old, next_node};
                mid_node->edges[remaining_new[0]] = {remaining_new, new PatriciaNode(true, data)};
                curr->edges[f_char] = {common, mid_node};
                return;
            }
        }
    }

private:
    PatriciaNode* new_node(bool leaf, int val) {
        PatriciaNode* n = new PatriciaNode();
        n->is_leaf = leaf;
        n->payload = val;
        return n;
    }
};

struct SplayNode {
    int key;
    SplayNode *left, *right, *parent;
    SplayNode(int k) : key(k), left(nullptr), right(nullptr), parent(nullptr) {}
};

class SplayTree {
private:
    SplayNode* root;

    void rotate_left(SplayNode* x) {
        SplayNode* y = x->right;
        if (!y) return;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rotate_right(SplayNode* x) {
        SplayNode* y = x->left;
        if (!y) return;
        x->left = y->right;
        if (y->right) y->right->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->right) x->parent->right = y;
        else x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    void splay(SplayNode* x) {
        while (x->parent) {
            SplayNode* p = x->parent;
            SplayNode* g = p->parent;
            if (!g) {
                if (x == p->left) rotate_right(p);
                else rotate_left(p);
            } else if (x == p->left && p == g->left) {
                rotate_right(g);
                rotate_right(p);
            } else if (x == p->right && p == g->right) {
                rotate_left(g);
                rotate_left(p);
            } else if (x == p->right && p == g->left) {
                rotate_left(p);
                rotate_right(g);
            } else {
                rotate_right(p);
                rotate_left(g);
            }
        }
    }

public:
    SplayTree() : root(nullptr) {}

    void insert(int key) {
        if (!root) {
            root = new SplayNode(key);
            return;
        }
        SplayNode* curr = root;
        SplayNode* p = nullptr;
        while (curr) {
            p = curr;
            if (key < curr->key) curr = curr->left;
            else if (key > curr->key) curr = curr->right;
            else { splay(curr); return; }
        }
        SplayNode* node = new SplayNode(key);
        node->parent = p;
        if (key < p->key) p->left = node;
        else p->right = node;
        splay(node);
    }

    bool search(int key) {
        if (!root) return false;
        SplayNode* curr = root;
        SplayNode* last = root;
        while (curr) {
            last = curr;
            if (key == curr->key) {
                splay(curr);
                return true;
            }
            curr = key < curr->key ? curr->left : curr->right;
        }
        splay(last);
        return false;
    }
};

struct TreapNode {
    int key;
    int priority;
    TreapNode *left, *right;
    TreapNode(int k, int p) : key(k), priority(p), left(nullptr), right(nullptr) {}
};

class Treap {
private:
    TreapNode* root;
    std::mt19937 rng;

    void split(TreapNode* t, int key, TreapNode*& l, TreapNode*& r) {
        if (!t) {
            l = r = nullptr;
        } else if (t->key <= key) {
            split(t->right, key, t->right, r);
            l = t;
        } else {
            split(t->left, key, l, t->left);
            r = t;
        }
    }

    void merge(TreapNode*& t, TreapNode* l, TreapNode* r) {
        if (!l || !r) {
            t = l ? l : r;
        } else if (l->priority > r->priority) {
            merge(l->right, l->right, r);
            t = l;
        } else {
            merge(r->left, l, r->left);
            t = r;
        }
    }

public:
    Treap() : root(nullptr), rng(1337) {}

    void insert(int key) {
        TreapNode* l = nullptr;
        TreapNode* r = nullptr;
        split(root, key, l, r);
        TreapNode* m = new TreapNode(key, rng());
        merge(l, l, m);
        merge(root, l, r);
    }
};

struct Point2D {
    double x, y;
};

struct KDNode {
    Point2D pt;
    KDNode *left, *right;
    KDNode(Point2D p) : pt(p), left(nullptr), right(nullptr) {}
};

class KDTree {
private:
    KDNode* root;

    KDNode* insert_rec(KDNode* node, Point2D pt, unsigned depth) {
        if (!node) return new KDNode(pt);
        unsigned cd = depth % 2;
        if (cd == 0) {
            if (pt.x < node->pt.x) node->left = insert_rec(node->left, pt, depth + 1);
            else node->right = insert_rec(node->right, pt, depth + 1);
        } else {
            if (pt.y < node->pt.y) node->left = insert_rec(node->left, pt, depth + 1);
            else node->right = insert_rec(node->right, pt, depth + 1);
        }
        return node;
    }

public:
    KDTree() : root(nullptr) {}

    void insert(Point2D pt) {
        root = insert_rec(root, pt, 0);
    }
};

int main() {
    const int num_elements = 100000;
    
    AdvancedTrie trie;
    PatriciaTree patricia;
    SplayTree splay;
    Treap treap;
    KDTree kdtree;

    std::mt19937 gen(42);
    std::uniform_int_distribution<int> dist(0, 1000000);
    std::uniform_real_distribution<double> dist_spatial(0.0, 1000.0);
auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_elements; ++i) {
        int val = dist(gen);
        splay.insert(val);
        treap.insert(val);
        kdtree.insert({dist_spatial(gen), dist_spatial(gen)});
        
        std::string s = "key_" + std::to_string(val);
        trie.insert(s, "action_" + std::to_string(i));
        patricia.insert(s, i);
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Benchmark finalizado com sucesso para " << num_elements << " iteracoes." << std::endl;
    std::cout << "Tempo total decorrido: " << elapsed.count() << " ms." << std::endl;

    return 0;
}
