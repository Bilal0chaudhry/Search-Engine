#pragma once
#include <string>
#include <vector>
#include "HashMapSearch.h"
#include "TrieSearch.h"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <unordered_set>

namespace fs = std::experimental::filesystem;

class SearchEngine {
private:
    HashMapSearch hashMapSearch;
    TrieSearch trieSearch;

    std::vector<std::string> splitQuery(const std::string& query) const;
    void displayResults(const std::vector<WordInDocument*>& results) const;

public:
    static bool useHashMap;
    static std::string normalize(const std::string& word);
    void indexDocuments(const std::string& folderPath, bool useHashMap, int numFiles);
    void indexDocument(const std::string& filePath, bool useHashMap);
    void searchQuery(const std::string& query, bool useHashMap) const;
    void comparePerformance(const std::vector<std::string>& queries);
    void clear(bool useHashMap);
    void save(const std::string& filePath, bool useHashMap);
    void load(const std::string& filePath, bool useHashMap);
    void displayIndex(bool useHashMap) const;
    static std::unordered_set<std::string> indexedFiles;
    bool dumpSearchEngine(const std::string& dumpFilePath);
    void addFileToIndex(const std::string& newFilePath, bool useHashMap);
    bool loadSearchEngine(const std::string& dumpFilePath);
};
