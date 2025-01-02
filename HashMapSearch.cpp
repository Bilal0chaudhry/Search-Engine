#include "HashMapSearch.h"
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

void HashMapSearch::insertWord(const std::string& word, const std::string& docName, int position) {
    auto& wordList = hashMap[word];  

    for (WordInDocument* wid : wordList) {
        if (wid->getDocName() == docName) {
            wid->addPosition(position); 
            return;
        }
    }

    wordList.push_back(new WordInDocument(docName, std::vector<int>{ position }));
}



std::vector<WordInDocument*> HashMapSearch::searchWord(const std::string& word) const {
    auto it = hashMap.find(word);
    return (it != hashMap.end()) ? it->second : std::vector<WordInDocument*>{};
}

std::vector<WordInDocument*> HashMapSearch::searchTwoWords(const std::string& word1, const std::string& word2) const {
    auto resultsWord1 = searchWord(word1);
    auto resultsWord2 = searchWord(word2);
    std::vector<WordInDocument*> result;
    std::unordered_map<std::string, WordInDocument*> docsMap;

    for (const auto& wid : resultsWord1) {
        docsMap[wid->getDocName()] = wid;
    }

    for (const auto& wid : resultsWord2) {
        if (docsMap.find(wid->getDocName()) != docsMap.end()) {
            result.push_back(docsMap[wid->getDocName()]);
        }
    }

    return result;
}

std::vector<WordInDocument*> HashMapSearch::searchMultipleWords(const std::vector<std::string>& words) const {
    if (words.empty()) {
        return {};
    }

    std::unordered_map<std::string, int> docCount;

    for (const auto& word : words) {
        auto results = searchWord(word);

        for (const auto& wid : results) {
            docCount[wid->getDocName()]++;
        }
    }

    std::vector<WordInDocument*> result;
    for (const auto& entry : docCount) {
        if (entry.second == words.size()) {
            for (const auto& word : words) {
                auto results = searchWord(word);
                for (const auto& wid : results) {
                    if (wid->getDocName() == entry.first) {
                        result.push_back(wid);
                        break;
                    }
                }
            }
        }
    }

    return result;
}

std::vector<WordInDocument*> HashMapSearch::searchExclusion(const std::string& word1, const std::string& word2) const {
    auto docsWithWord1 = searchWord(word1);
    auto docsWithWord2 = searchWord(word2);

    std::unordered_set<std::string> excludeDocs;
    for (const auto& doc : docsWithWord2) {
        excludeDocs.insert(doc->getDocName());
    }

    std::vector<WordInDocument*> result;
    for (const auto& doc : docsWithWord1) {
        if (excludeDocs.find(doc->getDocName()) == excludeDocs.end()) {
            result.push_back(doc);
        }
    }
    return result;
}

void HashMapSearch::clear() {
    for (auto& pair : hashMap) {
        for (WordInDocument* wid : pair.second) {
            delete wid;
        }
    }
    hashMap.clear();
}

void HashMapSearch::display() const {
    for (const auto& pair : hashMap) {
        std::cout << "Word: " << pair.first << std::endl;
        for (const WordInDocument* wid : pair.second) {
            std::cout << "  Document: " << wid->getDocName() << ", Positions: ";
            for (int pos : wid->getPositions()) {
                std::cout << pos << " ";
            }
            std::cout << std::endl;
        }
    }
}

void HashMapSearch::save(std::ofstream& outFile) const {
    if (!outFile.is_open()) {
        std::cerr << "Error: Output file stream is not open." << std::endl;
        return;
    }

    size_t mapSize = hashMap.size();
    outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));

    if (!outFile) {
        std::cerr << "Error: Failed to write map size." << std::endl;
        return;
    }

    for (const auto& pair : hashMap) {
        size_t wordLength = pair.first.size();
        outFile.write(reinterpret_cast<const char*>(&wordLength), sizeof(wordLength));

        if (!outFile) {
            std::cerr << "Error: Failed to write word length." << std::endl;
            return;
        }

        outFile.write(pair.first.c_str(), wordLength);

        if (!outFile) {
            std::cerr << "Error: Failed to write word." << std::endl;
            return;
        }

        size_t listSize = pair.second.size();
        outFile.write(reinterpret_cast<const char*>(&listSize), sizeof(listSize));

        if (!outFile) {
            std::cerr << "Error: Failed to write list size." << std::endl;
            return;
        }

        for (const WordInDocument* wid : pair.second) {
            wid->serialize(outFile);

            if (!outFile) {
                std::cerr << "Error: Failed to serialize WordInDocument." << std::endl;
                return;
            }
        }
    }

    std::cout << "HashMap successfully saved." << std::endl;
}


void HashMapSearch::load(std::ifstream& inFile) {
    if (!inFile.is_open()) {
        std::cerr << "Error: Input file stream is not open." << std::endl;
        return;
    }

    clear();

    size_t mapSize;
    inFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));
    if (!inFile) {
        std::cerr << "Error: Failed to read map size." << std::endl;
        return;
    }

    for (size_t i = 0; i < mapSize; ++i) {
        size_t wordLength;
        inFile.read(reinterpret_cast<char*>(&wordLength), sizeof(wordLength));
        if (!inFile) {
            std::cerr << "Error: Failed to read word length." << std::endl;
            return;
        }

        std::string word(wordLength, '\0');
        inFile.read(&word[0], wordLength);
        if (!inFile) {
            std::cerr << "Error: Failed to read word data." << std::endl;
            return;
        }

        size_t listSize;
        inFile.read(reinterpret_cast<char*>(&listSize), sizeof(listSize));
        if (!inFile) {
            std::cerr << "Error: Failed to read list size." << std::endl;
            return;
        }

        std::vector<WordInDocument*> wordList;
        for (size_t j = 0; j < listSize; ++j) {
            WordInDocument* wid = WordInDocument::deserialize(inFile);
            if (wid) {
                wordList.push_back(wid);
            }
            else {
                std::cerr << "Error: Failed to deserialize WordInDocument." << std::endl;
            }
        }

        hashMap[word] = wordList;
    }

    std::cout << "HashMap successfully loaded." << std::endl;
}


HashMapSearch::~HashMapSearch() {
    clear();
}
