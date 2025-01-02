#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include "WordInDocument.h"

class HashMapSearch {
private:
    std::unordered_map<std::string, std::vector<WordInDocument*>> hashMap;

public:
    void insertWord(const std::string& word, const std::string& docName, int position);
    std::vector<WordInDocument*> searchWord(const std::string& word) const;
    std::vector<WordInDocument*> searchTwoWords(const std::string& word1, const std::string& word2) const;
    std::vector<WordInDocument*> searchMultipleWords(const std::vector<std::string>& words) const;
    std::vector<WordInDocument*> searchExclusion(const std::string& word1, const std::string& word2) const;
    void clear();
    void display() const;
    void save(std::ofstream& outFile) const;
    void load(std::ifstream& inFile);
    ~HashMapSearch();
};
