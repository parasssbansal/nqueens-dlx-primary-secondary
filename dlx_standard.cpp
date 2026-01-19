#include <bits/stdc++.h>
using namespace std;

struct Column;

struct Node {
    Node *L, *R, *U, *D;
    Column *C;
    int rowID;
};

struct Column : Node {
    int size;
    string name;
};

class DLX {
public:
    Column *header;
    vector<Column*> columns;
    vector<Node*> solution;
    long long solutions = 0;

    DLX(int nCols) {
        header = new Column();
        header->L = header->R = header;
        for (int i = 0; i < nCols; i++) {
            Column *c = new Column();
            c->size = 0;
            c->name = to_string(i);
            c->U = c->D = c;
            c->C = c;
            c->R = header;
            c->L = header->L;
            header->L->R = c;
            header->L = c;
            columns.push_back(c);
        }
    }

    void addRow(int rID, const vector<int>& cols) {
        Node *prev = nullptr;
        for (int c : cols) {
            Column *col = columns[c];
            Node *node = new Node();
            node->rowID = rID;
            node->C = col;

            node->D = col;
            node->U = col->U;
            col->U->D = node;
            col->U = node;
            col->size++;

            if (!prev) {
                node->L = node->R = node;
            } else {
                node->R = prev->R;
                node->L = prev;
                prev->R->L = node;
                prev->R = node;
            }
            prev = node;
        }
    }

    void cover(Column *c) {
        c->R->L = c->L;
        c->L->R = c->R;
        for (Node *i = c->D; i != c; i = i->D) {
            for (Node *j = i->R; j != i; j = j->R) {
                j->D->U = j->U;
                j->U->D = j->D;
                j->C->size--;
            }
        }
    }

    void uncover(Column *c) {
        for (Node *i = c->U; i != c; i = i->U) {
            for (Node *j = i->L; j != i; j = j->L) {
                j->C->size++;
                j->D->U = j;
                j->U->D = j;
            }
        }
        c->R->L = c;
        c->L->R = c;
    }

    void search() {
        if (header->R == header) {
            solutions++;
            return;
        }

        Column *c = nullptr;
        int minSize = INT_MAX;
        for (Column *j = (Column*)header->R; j != header; j = (Column*)j->R) {
            if (j->size < minSize) {
                minSize = j->size;
                c = j;
            }
        }

        cover(c);
        for (Node *r = c->D; r != c; r = r->D) {
            solution.push_back(r);
            for (Node *j = r->R; j != r; j = j->R)
                cover(j->C);

            search();

            for (Node *j = r->L; j != r; j = j->L)
                uncover(j->C);
            solution.pop_back();
        }
        uncover(c);
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: ./program <N>\n";
        cout << "Example: ./program 8\n";
        return 1;
    }

    int N = stoi(argv[1]);

    int cols = 6*N - 2;
    DLX dlx(cols);

    int diagOffset = N - 1;
    int diagStart = 2*N;
    int antiStart = 2*N + (2*N - 1);

    int rowID = 0;
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            vector<int> row = {
                r,
                N + c,
                diagStart + (r - c + diagOffset),
                antiStart + (r + c)
            };
            dlx.addRow(rowID++, row);
        }
    }

    dlx.search();
    cout << "Solutions: " << dlx.solutions << endl;
    return 0;
}
