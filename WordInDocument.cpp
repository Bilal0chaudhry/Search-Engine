#include "WordInDocument.h"
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

WordInDocument::WordInDocument(const string& docName, const vector<int>& positions)
    : docName(docName), positions(positions) {
}

void WordInDocument::addPosition(int position) {
    positions.push_back(position);
}

string WordInDocument::getDocName() const {
    return docName;
}

vector<int> WordInDocument::getPositions() const {
    return positions;
}

int WordInDocument::getFrequency() const {
    return positions.size();
}

string WordInDocument::toString() const {
    ostringstream oss;
    oss << "Document: " << docName << ", Frequency: " << getFrequency() << ", Positions: [";
    for (size_t i = 0; i < positions.size(); ++i) {
        oss << positions[i];
        if (i != positions.size() - 1) {
            oss << ", ";
        }
    }
    oss << "]";
    return oss.str();
}

void WordInDocument::display() const {
    cout << toString() << endl;
}

void WordInDocument::serialize(ofstream& outFile) const {
    size_t docNameLength = docName.size();
    outFile.write(reinterpret_cast<const char*>(&docNameLength), sizeof(docNameLength));
    outFile.write(docName.c_str(), docNameLength);

    size_t positionsSize = positions.size();
    outFile.write(reinterpret_cast<const char*>(&positionsSize), sizeof(positionsSize));
    for (int position : positions) {
        outFile.write(reinterpret_cast<const char*>(&position), sizeof(position));
    }
}

WordInDocument* WordInDocument::deserialize(ifstream& inFile) {
    size_t docNameLength;
    inFile.read(reinterpret_cast<char*>(&docNameLength), sizeof(docNameLength));

    string docName(docNameLength, '\0');
    inFile.read(&docName[0], docNameLength);

    size_t positionsSize;
    inFile.read(reinterpret_cast<char*>(&positionsSize), sizeof(positionsSize));

    vector<int> positions(positionsSize);
    for (size_t i = 0; i < positionsSize; ++i) {
        inFile.read(reinterpret_cast<char*>(&positions[i]), sizeof(positions[i]));
    }

    return new WordInDocument(docName, positions);
}

