#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <limits>

using namespace std;

class FibonacciHeap {
private:
    class Node {
    public:
        int key;
        int degree;
        Node* left_sibling;
        Node* right_sibling;
        Node* parent;
        Node* child;
        bool mark;

        Node(int k) : key(k), degree(0), left_sibling(this), right_sibling(this),
                      parent(nullptr), child(nullptr), mark(false) {}
    };

    int n;
    Node* min;

    void consolidate();
    void fib_heap_link(Node* y, Node* x);
    void cut(Node* node_to_be_decreased, Node* parent_node);
    void cascading_cut(Node* parent_node);
    void find_and_decrease_key(Node* n, int old_key, int new_key);
    int calculate_degree(int n) const;
    void print_heap_recursive(Node* n) const;

public:
    FibonacciHeap() : n(0), min(nullptr) {}
    ~FibonacciHeap();

    void insert(int val);
    int find_min() const;
    Node* extract_min();
    void decrease_key(int old_key, int new_key);
    void delete_node(int key);
    void print_heap() const;
    void union_with(FibonacciHeap& other);

private:
    void cleanup(Node* node);
};

FibonacciHeap::~FibonacciHeap() {
    if (min) {
        cleanup(min);
    }
}

void FibonacciHeap::cleanup(Node* node) {
    if (!node) return;

    Node* current = node;
    do {
        Node* temp = current;
        if (current->child) {
            cleanup(current->child);
        }
        current = current->right_sibling;
        delete temp;
    } while (current != node);
}

void FibonacciHeap::insert(int val) {
    Node* new_node = new Node(val);

    if (!min) {
        min = new_node;
    } else {
        min->left_sibling->right_sibling = new_node;
        new_node->right_sibling = min;
        new_node->left_sibling = min->left_sibling;
        min->left_sibling = new_node;

        if (new_node->key < min->key) {
            min = new_node;
        }
    }
    n++;
}

int FibonacciHeap::find_min() const {
    if (!min) {
        throw runtime_error("Heap is empty");
    }
    return min->key;
}

void FibonacciHeap::print_heap() const {
    if (!min) {
        cout << "Empty heap" << endl;
        return;
    }
    print_heap_recursive(min);
}

void FibonacciHeap::print_heap_recursive(Node* n) const {
    Node* x = n;
    do {
        if (!x->child) {
            cout << "Node with no child (" << x->key << ")" << endl;
        } else {
            cout << "Node(" << x->key << ") with child (" << x->child->key << ")" << endl;
            print_heap_recursive(x->child);
        }
        x = x->right_sibling;
    } while (x != n);
}

int FibonacciHeap::calculate_degree(int n) const {
    int count = 0;
    while (n > 0) {
        n = n / 2;
        count++;
    }
    return count;
}

void FibonacciHeap::consolidate() {
    int max_degree = calculate_degree(n);
    vector<Node*> A(max_degree + 1, nullptr);

    Node* x = min;
    if (!x) return;

    vector<Node*> root_list;
    Node* current = x;
    do {
        root_list.push_back(current);
        current = current->right_sibling;
    } while (current != x);

    for (Node* current : root_list) {
        int d = current->degree;
        while (A[d] != nullptr) {
            Node* y = A[d];
            if (current->key > y->key) {
                swap(current, y);
            }
            fib_heap_link(y, current);
            A[d] = nullptr;
            d++;
        }
        A[d] = current;
    }

    min = nullptr;
    for (Node* node : A) {
        if (node) {
            node->left_sibling = node->right_sibling = node;
            if (!min) {
                min = node;
            } else {
                min->left_sibling->right_sibling = node;
                node->right_sibling = min;
                node->left_sibling = min->left_sibling;
                min->left_sibling = node;
                if (node->key < min->key) {
                    min = node;
                }
            }
        }
    }
}

void FibonacciHeap::fib_heap_link(Node* y, Node* x) {
    y->right_sibling->left_sibling = y->left_sibling;
    y->left_sibling->right_sibling = y->right_sibling;

    if (x->right_sibling == x) {
        min = x;
    }

    y->left_sibling = y->right_sibling = y;
    y->parent = x;

    if (!x->child) {
        x->child = y;
    } else {
        y->right_sibling = x->child;
        y->left_sibling = x->child->left_sibling;
        x->child->left_sibling->right_sibling = y;
        x->child->left_sibling = y;
        if (y->key < x->child->key) {
            x->child = y;
        }
    }

    x->degree++;
}

FibonacciHeap::Node* FibonacciHeap::extract_min() {
    if (!min) {
        throw runtime_error("Heap is empty");
    }

    Node* temp = min;
    if (temp->child) {
        Node* child = temp->child;
        do {
            Node* next = child->right_sibling;
            min->left_sibling->right_sibling = child;
            child->right_sibling = min;
            child->left_sibling = min->left_sibling;
            min->left_sibling = child;
            child->parent = nullptr;
            child = next;
        } while (child != temp->child);
    }

    temp->left_sibling->right_sibling = temp->right_sibling;
    temp->right_sibling->left_sibling = temp->left_sibling;

    if (temp == temp->right_sibling) {
        min = nullptr;
    } else {
        min = temp->right_sibling;
        consolidate();
    }

    n--;
    cout << "Extracted minimum: " << temp->key << endl; // Print the extracted minimum
    return temp;
}

void FibonacciHeap::decrease_key(int old_key, int new_key) {
    if (min) {
        find_and_decrease_key(min, old_key, new_key);
    }
}

void FibonacciHeap::find_and_decrease_key(Node* n, int old_key, int new_key) {
    if (!n) return;

    Node* current = n;
    do {
        if (current->key == old_key) {
            if (new_key > old_key) {
                throw runtime_error("New key is greater than current key");
            }
            current->key = new_key;
            Node* parent = current->parent;
            if (parent && current->key < parent->key) {
                cut(current, parent);
                cascading_cut(parent);
            }
            if (current->key < min->key) {
                min = current;
            }
            return;
        }
        if (current->child) {
            find_and_decrease_key(current->child, old_key, new_key);
        }
        current = current->right_sibling;
    } while (current != n);
}

void FibonacciHeap::cut(Node* node_to_be_decreased, Node* parent_node) {
    if (node_to_be_decreased == node_to_be_decreased->right_sibling) {
        parent_node->child = nullptr;
    } else {
        node_to_be_decreased->left_sibling->right_sibling = node_to_be_decreased->right_sibling;
        node_to_be_decreased->right_sibling->left_sibling = node_to_be_decreased->left_sibling;
        if (node_to_be_decreased == parent_node->child) {
            parent_node->child = node_to_be_decreased->right_sibling;
        }
    }

    parent_node->degree--;
    node_to_be_decreased->right_sibling = min;
    node_to_be_decreased->left_sibling = min->left_sibling;
    min->left_sibling->right_sibling = node_to_be_decreased;
    min->left_sibling = node_to_be_decreased;
    node_to_be_decreased->parent = nullptr;
    node_to_be_decreased->mark = false;
}

void FibonacciHeap::cascading_cut(Node* parent_node) {
    Node* aux = parent_node->parent;
    if (aux) {
        if (!parent_node->mark) {
            parent_node->mark = true;
        } else {
            cut(parent_node, aux);
            cascading_cut(aux);
        }
    }
}

void FibonacciHeap::delete_node(int key) {
    decrease_key(key, numeric_limits<int>::min());
    extract_min();
}

void FibonacciHeap::union_with(FibonacciHeap& other) {
    if (!other.min) return;
    if (!min) {
        min = other.min;
        n = other.n;
    } else {
        Node* temp1 = min->right_sibling;
        Node* temp2 = other.min->left_sibling;

        min->right_sibling = other.min;
        other.min->left_sibling = min;
        temp1->left_sibling = temp2;
        temp2->right_sibling = temp1;

        if (other.min->key < min->key) {
            min = other.min;
        }
        n += other.n;
    }
    other.min = nullptr;
    other.n = 0;
}

int main() {
    FibonacciHeap heap;
    FibonacciHeap heap2;

    int choice;
    do {
        cout << "\nMenu:" << endl;
        cout << "1. Insert into Heap" << endl;
        cout << "2. Find Minimum" << endl;
        cout << "3. Extract Minimum" << endl;
        cout << "4. Decrease Key" << endl;
        cout << "5. Delete Node" << endl;
        cout << "6. Union Heaps" << endl;
        cout << "7. Print Heap" << endl;
        cout << "8. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            int val;
            cout << "Enter value to insert: ";
            cin >> val;
            heap.insert(val);
            break;
        }
        case 2:
            try {
                cout << "Minimum value: " << heap.find_min() << endl;
            } catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
            break;
        case 3:
            try {
                heap.extract_min();
            } catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
            break;
        case 4: {
            int old_key, new_key;
            cout << "Enter key to decrease: ";
            cin >> old_key;
            cout << "Enter new key value: ";
            cin >> new_key;
            try {
                heap.decrease_key(old_key, new_key);
            } catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
            break;
        }
        case 5: {
            int del_key;
            cout << "Enter key to delete: ";
            cin >> del_key;
            try {
                heap.delete_node(del_key);
            } catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
            break;
        }
        case 6:
            cout << "\nEnter number of nodes for second heap: ";
            int no_of_nodes;
            cin >> no_of_nodes;
            for (int i = 1; i <= no_of_nodes; i++) {
                int ele;
                cout << "Node " << i << " and its key value: ";
                cin >> ele;
                heap2.insert(ele);
            }
            heap.union_with(heap2);
            cout << "Heaps united." << endl;
            break;
        case 7:
            cout << "Heap contents:" << endl;
            heap.print_heap();
            break;
        case 8:
            cout << "Exiting program." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 8);

    return 0;
}
