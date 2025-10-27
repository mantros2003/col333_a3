#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_set>
#include "common.h"

using namespace std;

int N, M, K, J;
vector<MetroLine> metroLines;

int occ_var_base = 0;
int edge_var_base = 0;
int turn_var_base = 0;

const int UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3;

int get_edge_var(int k, int r, int c, int dir) {
    return edge_var_base + 1 + k * (M * N * 4) + r * (N * 4) + c * 4 + dir;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: ./decoder <input.city> <input.satoutput> <output.metromap>" << endl;
        return 1;
    }

    ifstream fcity(argv[1]);
    string line;
    
    // Read scenario
    int scenario;
    getline(fcity, line);
    stringstream ss(line);
    ss >> scenario;

    // Read problem parameters
    getline(fcity, line);
    ss.clear();
    ss.str(line);
    ss >> N >> M >> K >> J;
    
    // Read metro lines using the struct
    for (int k = 0; k < K; ++k) {
        getline(fcity, line);
        ss.clear();
        ss.str(line);
        MetroLine ml;
        ss >> ml.startX >> ml.startY >> ml.endX >> ml.endY;
        metroLines.push_back(ml);
    }
    fcity.close();
    
    // Initialize variable bases
    occ_var_base = 0;
    edge_var_base = K * M * N;
    turn_var_base = K * M * N + K * M * N * 4;

    ifstream fsat(argv[2]);
    string result;
    fsat >> result;

    ofstream fmap(argv[3]);
    if (result == "UNSAT") {
        fmap << "0" << endl;
        fsat.close();
        fmap.close();
        return 0;
    }

    unordered_set<int> true_vars;
    int lit;
    while (fsat >> lit && lit != 0) {
        if (lit > 0) {
            true_vars.insert(lit);
        }
    }
    fsat.close();

    for (int k = 0; k < K; ++k) {
        string path = "";
        // Use the fields from the MetroLine struct
        int r = metroLines[k].startY;
        int c = metroLines[k].startX;
        int end_r = metroLines[k].endY;
        int end_c = metroLines[k].endX;

        while (r != end_r || c != end_c) {
            bool found_move = false;
            // Check Up
            if (r > 0 && true_vars.count(get_edge_var(k, r, c, UP))) {
                path += "U "; r--; found_move = true;
            } 
            // Check Right
            else if (c < N - 1 && true_vars.count(get_edge_var(k, r, c, RIGHT))) {
                path += "R "; c++; found_move = true;
            } 
            // Check Down
            else if (r < M - 1 && true_vars.count(get_edge_var(k, r, c, DOWN))) {
                path += "D "; r++; found_move = true;
            } 
            // Check Left
            else if (c > 0 && true_vars.count(get_edge_var(k, r, c, LEFT))) {
                path += "L "; c--; found_move = true;
            }
            
            if (!found_move) {
                break;
            }
        }
        fmap << path << "0" << endl;
    }
    fmap.close();

    return 0;
}