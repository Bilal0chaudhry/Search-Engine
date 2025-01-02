#pragma once
#include <string>
#include <vector>
#include <fstream>

class WordInDocument {
public:
    std::string docName;
    std::vector<int> positions;

    WordInDocument(const std::string& docName, const std::vector<int>& positions);

    void addPosition(int position);

    std::string getDocName() const;

    std::vector<int> getPositions() const;

    int getFrequency() const;

    std::string toString() const;

    void display() const;

    void serialize(std::ofstream& outFile) const;

    static WordInDocument* deserialize(std::ifstream& inFile);
};
