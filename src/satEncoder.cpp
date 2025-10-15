#include <map>
#include <vector>
#include <fstream>
#include <algorithm>
#include <functional>
#include "common.h"
#include "satEncoder.h"

// Directions: 0:Up, 1:Right, 2:Down, 3:Left
const int UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3;

using namespace std;

void basicSATGen(int N, int M, int K, int J, const vector<MetroLine>& metroLines, const vector<Point>& popularCells, const string& filename) {
    // Variables from where the counting starts
    int occ_var_base = 0;
    int edge_var_base = K * M * N;
    int turn_var_base = K * M * N + K * M * N * 4;
    int total_vars = turn_var_base + K * M * N;

    auto get_occ_var = [&](int k, int r, int c) {
        return occ_var_base + 1 + k * (M * N) + r * N + c;
    };
    auto get_edge_var = [&](int k, int r, int c, int dir) {
        return edge_var_base + 1 + k * (M * N * 4) + r * (N * 4) + c * 4 + dir;
    };
    auto get_turn_var = [&](int k, int r, int c) {
        return turn_var_base + 1 + k * (M * N) + r * N + c;
    };

    vector<vector<int>> clauses;

    // Generate clauses
    for (int k = 0; k < K; ++k) {
        int start_r = metroLines[k].startY;
        int start_c = metroLines[k].startX;
        int end_r = metroLines[k].endY;
        int end_c = metroLines[k].endX;

        // Start and end points must be occupied
        clauses.push_back({get_occ_var(k, start_r, start_c)});
        clauses.push_back({get_occ_var(k, end_r, end_c)});

        for (int r = 0; r < M; ++r) {
            for (int c = 0; c < N; ++c) {
                vector<int> connected_edges;
                if (r > 0) connected_edges.push_back(get_edge_var(k, r - 1, c, DOWN));
                if (c < N - 1) connected_edges.push_back(get_edge_var(k, r, c + 1, LEFT));
                if (r < M - 1) connected_edges.push_back(get_edge_var(k, r + 1, c, UP));
                if (c > 0) connected_edges.push_back(get_edge_var(k, r, c - 1, RIGHT));
                if (r > 0) connected_edges.push_back(get_edge_var(k, r, c, UP));
                if (c < N - 1) connected_edges.push_back(get_edge_var(k, r, c, RIGHT));
                if (r < M - 1) connected_edges.push_back(get_edge_var(k, r, c, DOWN));
                if (c > 0) connected_edges.push_back(get_edge_var(k, r, c, LEFT));

                int occ_var = get_occ_var(k, r, c);
                vector<int> backward_clause = {-occ_var};
                for (int edge_var : connected_edges) {
                    // Edge => Occupied
                    clauses.push_back({-edge_var, occ_var});
                    backward_clause.push_back(edge_var);
                }
                if (!connected_edges.empty()) clauses.push_back(backward_clause);
                else clauses.push_back({-occ_var});

                bool is_start = (r == start_r && c == start_c);
                bool is_end = (r == end_r && c == end_c);
                vector<int> outgoing_edges, incoming_edges;
                if (r > 0) outgoing_edges.push_back(get_edge_var(k,r,c,UP));
                if (c < N-1) outgoing_edges.push_back(get_edge_var(k,r,c,RIGHT));
                if (r < M-1) outgoing_edges.push_back(get_edge_var(k,r,c,DOWN));
                if (c > 0) outgoing_edges.push_back(get_edge_var(k,r,c,LEFT));
                if (r > 0) incoming_edges.push_back(get_edge_var(k,r-1,c,DOWN));
                if (c < N-1) incoming_edges.push_back(get_edge_var(k,r,c+1,LEFT));
                if (r < M-1) incoming_edges.push_back(get_edge_var(k,r+1,c,UP));
                if (c > 0) incoming_edges.push_back(get_edge_var(k,r,c-1,RIGHT));

                if (is_start) {
                    if(!outgoing_edges.empty()) clauses.push_back(outgoing_edges);
                    // Only one outgoing edge
                    for (size_t i = 0; i < outgoing_edges.size(); ++i) {
                        for (size_t j = i + 1; j < outgoing_edges.size(); ++j) {
                            clauses.push_back({-outgoing_edges[i], -outgoing_edges[j]});
                        }
                    }
                    // No in-edge for start cell
                    for (int edge : incoming_edges) clauses.push_back({-edge});
                } else if (is_end) {
                    if(!incoming_edges.empty()) clauses.push_back(incoming_edges);
                    for (size_t i = 0; i < incoming_edges.size(); ++i) {
                        for (size_t j = i + 1; j < incoming_edges.size(); ++j) {
                            clauses.push_back({-incoming_edges[i], -incoming_edges[j]});
                        }
                    }
                    // No out-edge for end
                    for (int edge: outgoing_edges) clauses.push_back({-edge});
                } else {
                    // In-edge => Out-edge
                    for (int in_edge: incoming_edges) {
                        vector<int> clause = {-in_edge};
                        for(int out_edge: outgoing_edges) clause.push_back(out_edge);
                        clauses.push_back(clause);
                    }
                    // Out-edge => In-edge
                    for (int out_edge: outgoing_edges) {
                        vector<int> clause = {-out_edge};
                        for(int in_edge: incoming_edges) clause.push_back(in_edge);
                        clauses.push_back(clause);
                    }
                    // Only one incoming edge for occupied intermediate cell
                    for (size_t i = 0; i < incoming_edges.size(); ++i) {
                        for (size_t j = i + 1; j < incoming_edges.size(); ++j) {
                            clauses.push_back({-occ_var, -incoming_edges[i], -incoming_edges[j]});
                        }
                    }
                    // Only one outgoing cell for occupied intermediate cell
                    for (size_t i = 0; i < outgoing_edges.size(); ++i) {
                        for (size_t j = i + 1; j < outgoing_edges.size(); ++j) {
                            clauses.push_back({-occ_var, -outgoing_edges[i], -outgoing_edges[j]});
                        }
                    }
                }
                
                int turn_var = get_turn_var(k,r,c);
                int in_up = (r > 0) ? get_edge_var(k,r-1,c,DOWN) : 0;
                int in_right = (c < N-1) ? get_edge_var(k,r,c+1,LEFT) : 0;
                int in_down = (r < M-1) ? get_edge_var(k,r+1,c,UP) : 0;
                int in_left = (c > 0) ? get_edge_var(k,r,c-1,RIGHT) : 0;

                int out_up = (r > 0) ? get_edge_var(k,r,c,UP) : 0;
                int out_right = (c < N-1) ? get_edge_var(k,r,c,RIGHT) : 0;
                int out_down = (r < M-1) ? get_edge_var(k,r,c,DOWN) : 0;
                int out_left = (c > 0) ? get_edge_var(k,r,c,LEFT) : 0;

                vector<pair<int, int>> turn_pairs;
                if(in_up && out_right) turn_pairs.push_back({in_up, out_right});
                if(in_up && out_left) turn_pairs.push_back({in_up, out_left});
                if(in_right && out_up) turn_pairs.push_back({in_right, out_up});
                if(in_right && out_down) turn_pairs.push_back({in_right, out_down});
                if(in_down && out_right) turn_pairs.push_back({in_down, out_right});
                if(in_down && out_left) turn_pairs.push_back({in_down, out_left});
                if(in_left && out_up) turn_pairs.push_back({in_left, out_up});
                if(in_left && out_down) turn_pairs.push_back({in_left, out_down});
                
                // (orthogonal in and out) => turn
                // not in or not out or turn
                for (auto p: turn_pairs) {
                    clauses.push_back({-p.first, -p.second, turn_var});
                }
            }
        }
        vector<int> all_turn_vars_for_k;
        for (int r = 0; r < M; ++r)
            for(int c = 0; c < N; ++c)
                all_turn_vars_for_k.push_back(get_turn_var(k, r, c));
        
        if (J < (M-1)*(N-1) && !all_turn_vars_for_k.empty()) {
            vector<int> p(all_turn_vars_for_k.size(), 0);
            if (J + 1 <= (int)p.size()) {
                fill(p.begin(), p.begin() + J + 1, 1);
                do {
                    vector<int> clause;
                    for(size_t i=0; i<p.size(); ++i){
                        if(p[i]) clause.push_back(-all_turn_vars_for_k[i]);
                    }
                    clauses.push_back(clause);
                } while(prev_permutation(p.begin(), p.end()));
            }
        }
    }

    for (const auto& cell : popularCells) {
        vector<int> popular_clause;
        for (int k = 0; k < K; ++k) {
            popular_clause.push_back(get_occ_var(k, cell.y, cell.x));
        }
        if(!popular_clause.empty()) {
            clauses.push_back(popular_clause);
        }
    }

    for (int r = 0; r < M; ++r) {
        for (int c = 0; c < N; ++c) {
            for (int k1 = 0; k1 < K; ++k1) {
                for (int k2 = k1 + 1; k2 < K; ++k2) {
                    clauses.push_back({-get_occ_var(k1, r, c), -get_occ_var(k2, r, c)});
                }
            }
        }
    }
    
    ofstream fout(filename);
    fout << "p cnf " << total_vars << " " << clauses.size() << endl;
    for (const auto& clause : clauses) {
        for (int lit : clause) {
            fout << lit << " ";
        }
        fout << "0" << endl;
    }
    fout.close();
}