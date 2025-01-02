#pragma once
#include <vector>
#include <string>
#include "WordInDocument.h"
#include <fstream>

class TrieNode {
public:
    char value;
    TrieNode* children[256];
    bool isEndOfWord;
    std::vector<WordInDocument*> wordOccurrences;

    TrieNode(char val);
    ~TrieNode();
};

class TrieSearch {
private:
    TrieNode* root;

    int charToIndex(char c) const;
    void displayHelper(TrieNode* node, std::string currentWord) const;
    TrieNode* getNode(const std::string& word) const;

    void saveHelper(std::ofstream& outFile, TrieNode* node) const;
    void loadHelper(std::ifstream& inFile, TrieNode* node);

public:
    TrieSearch();
    ~TrieSearch();

    void insertWord(const std::string& word, const std::string& docName, int position);
    std::vector<WordInDocument*> searchWord(const std::string& word) const;
    void clear();
    void clearHelper(TrieNode* node);
    void display() const;

    void save(std::ofstream& outFile) const;
    void load(std::ifstream& inFile);
    std::vector<WordInDocument*> searchExclusion(const std::string& word1, const std::string& word2) const;
    std::vector<WordInDocument*> searchMultipleWords(const std::vector<std::string>& includedWords) const;
};
