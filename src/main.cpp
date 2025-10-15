#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "common.h"
#include "satEncoder.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename = std::string(argv[1]) + ".city";

    std::ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file '" << argv[1] << "'" << std::endl;
        return 1;
    }

    std::string line;

    int scenario;
    if (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        ss >> scenario;
    } else {
        std::cerr << "Error: Could not read scenario from file." << std::endl;
        return 1;
    }

    int N, M, K, J;
    int P = 0;
    if (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        if (scenario == 1) {
            ss >> N >> M >> K >> J;
        } else if (scenario == 2) {
            ss >> N >> M >> K >> J >> P;
        } else {
            std::cerr << "Error: Unknown scenario number " << scenario << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Error: Could not read grid parameters from file." << std::endl;
        return 1;
    }

    std::vector<MetroLine> metroLines;
    for (int i = 0; i < K; ++i) {
        if (std::getline(inputFile, line)) {
            std::stringstream ss(line);
            MetroLine ml;
            ss >> ml.startX >> ml.startY >> ml.endX >> ml.endY;
            metroLines.push_back(ml);
        } else {
            std::cerr << "Error: Reached end of file while expecting more metro lines." << std::endl;
            return 1;
        }
    }

    std::vector<Point> popularCells;
    if (scenario == 2) {
        if (std::getline(inputFile, line)) {
            std::stringstream ss(line);
            for (int i = 0; i < P; ++i) {
                Point p;
                if (ss >> p.x >> p.y) {
                    popularCells.push_back(p);
                } else {
                    std::cerr << "Error: Could not read all popular cell coordinates." << std::endl;
                    return 1;
                }
            }
        } else {
            std::cerr << "Error: Reached end of file while expecting popular cells line." << std::endl;
            return 1;
        }
    }

    inputFile.close();

    return 0;
}