#include "TrieSearch.h"
#include <iostream>
#include <cctype>
#include "SearchEngine.h"
#include <fstream>
#include <unordered_set>
#include <unordered_map>
using namespace std;

TrieNode::TrieNode(char val) : value(val), isEndOfWord(false) {
    for (int i = 0; i < 256; ++i) {
        children[i] = nullptr;
    }
}

TrieNode::~TrieNode() {
    for (int i = 0; i < 256; ++i) {
        if (children[i] != nullptr) {
            delete children[i];
        }
    }
}

TrieSearch::TrieSearch() {
    root = new TrieNode('\0');
}

TrieSearch::~TrieSearch() {
    clear();
    delete root;
}

int TrieSearch::charToIndex(char c) const {
    return static_cast<unsigned char>(c);
}

void TrieSearch::insertWord(const string& word, const string& docName, int position) {
    TrieNode* current = root;

    for (char c : word) {
        int index = charToIndex(c);
        if (current->children[index] == nullptr) {
            current->children[index] = new TrieNode(c);
        }
        current = current->children[index];
    }

    current->isEndOfWord = true;

    for (WordInDocument* doc : current->wordOccurrences) {
        if (doc->getDocName() == docName) {
            doc->addPosition(position);
            return;
        }
    }

    vector<int> positions = { position };
    current->wordOccurrences.push_back(new WordInDocument(docName, positions));
}

std::vector<WordInDocument*> TrieSearch::searchWord(const std::string& word) const {
    string normalizedWord = SearchEngine::normalize(word);
    TrieNode* node = getNode(normalizedWord);

    if (node != nullptr && node->isEndOfWord) {
        std::vector<WordInDocument*> sortedDocs = node->wordOccurrences;
        for (size_t i = 0; i < sortedDocs.size(); ++i) {
            for (size_t j = i + 1; j < sortedDocs.size(); ++j) {
                if (sortedDocs[i]->getPositions().size() < sortedDocs[j]->getPositions().size()) {
                    WordInDocument* temp = sortedDocs[i];
                    sortedDocs[i] = sortedDocs[j];
                    sortedDocs[j] = temp;
                }
            }
        }
        return sortedDocs;
    }
    return {};
}

TrieNode* TrieSearch::getNode(const string& word) const {
    TrieNode* current = root;

    for (char c : word) {
        int index = charToIndex(c);
        if (current->children[index] == nullptr) {
            return nullptr;
        }
        current = current->children[index];
    }

    return current;
}

void TrieSearch::display() const {
    displayHelper(root, "");
}

void TrieSearch::displayHelper(TrieNode* node, string currentWord) const {
    if (node->isEndOfWord) {
        cout << "Word: " << currentWord << "\n";
        for (auto& doc : node->wordOccurrences) {
            cout << "  Document: " << doc->getDocName() << ", Positions: ";
            for (int pos : doc->getPositions()) {
                cout << pos << " ";
            }
            cout << "\n";
        }
    }

    for (int i = 0; i < 256; ++i) {
        if (node->children[i] != nullptr) {
            displayHelper(node->children[i], currentWord + node->children[i]->value);
        }
    }
}

void TrieSearch::clear() {
    clearHelper(root);
    root = nullptr; 
}

void TrieSearch::clearHelper(TrieNode* node) {
    if (node == nullptr) return;

    for (int i = 0; i < 256; ++i) {
        if (node->children[i] != nullptr) {
            clearHelper(node->children[i]);  
            delete node->children[i];        
            node->children[i] = nullptr;    
        }
    }
}


void TrieSearch::save(std::ofstream& outFile) const {
    if (!outFile.is_open()) {
        std::cerr << "Error: Output file stream is not open." << std::endl;
        return;
    }

    try {
        saveHelper(outFile, root);

        if (!outFile) {
            std::cerr << "Error: Failed to save Trie." << std::endl;
            return;
        }

        std::cout << "Trie successfully saved." << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error during save: " << ex.what() << std::endl;
    }
}

void TrieSearch::saveHelper(std::ofstream& outFile, TrieNode* node) const {
    outFile.write(reinterpret_cast<const char*>(&node->value), sizeof(node->value));
    if (!outFile) throw std::runtime_error("Failed to write node->value");

    outFile.write(reinterpret_cast<const char*>(&node->isEndOfWord), sizeof(node->isEndOfWord));
    if (!outFile) throw std::runtime_error("Failed to write node->isEndOfWord");

    size_t occurrencesSize = node->wordOccurrences.size();
    outFile.write(reinterpret_cast<const char*>(&occurrencesSize), sizeof(occurrencesSize));
    if (!outFile) throw std::runtime_error("Failed to write occurrencesSize");

    for (const WordInDocument* doc : node->wordOccurrences) {
        doc->serialize(outFile);
        if (!outFile) throw std::runtime_error("Failed to write WordInDocument");
    }

    for (int i = 0; i < 256; ++i) {
        bool hasChild = (node->children[i] != nullptr);
        outFile.write(reinterpret_cast<const char*>(&hasChild), sizeof(hasChild)); 
        if (!outFile) throw std::runtime_error("Failed to write child flag");

        if (hasChild) {
            saveHelper(outFile, node->children[i]); 
        }
    }
}


void TrieSearch::load(std::ifstream& inFile) {
    if (!inFile.is_open()) {
        std::cerr << "Error: Input file stream is not open." << std::endl;
        return;
    }

    clear(); 

    try {
        loadHelper(inFile, root);  
        std::cout << "Trie successfully loaded." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error while loading Trie: " << e.what() << std::endl;
    }
}


void TrieSearch::loadHelper(std::ifstream& inFile, TrieNode* node) {
    inFile.read(reinterpret_cast<char*>(&node->value), sizeof(node->value));
    inFile.read(reinterpret_cast<char*>(&node->isEndOfWord), sizeof(node->isEndOfWord));

    size_t occurrencesSize;
    inFile.read(reinterpret_cast<char*>(&occurrencesSize), sizeof(occurrencesSize));
    for (size_t i = 0; i < occurrencesSize; ++i) {
        WordInDocument* doc = WordInDocument::deserialize(inFile);
        node->wordOccurrences.push_back(doc);
    }

    for (int i = 0; i < 256; ++i) {
        bool hasChild;
        inFile.read(reinterpret_cast<char*>(&hasChild), sizeof(hasChild));

        if (hasChild) {
            TrieNode* childNode = new TrieNode('\0'); 
            node->children[i] = childNode;          
            loadHelper(inFile, childNode);          
        }
    }
}


std::vector<WordInDocument*> TrieSearch::searchExclusion(const std::string& word1, const std::string& word2) const {
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

std::vector<WordInDocument*> TrieSearch::searchMultipleWords(const std::vector<std::string>& includedWords) const {
    std::unordered_map<std::string, int> docCount;

    for (const auto& word : includedWords) {
        auto results = searchWord(word);
        for (const auto& wid : results) {
            docCount[wid->getDocName()]++;
        }
    }

    std::vector<WordInDocument*> result;

    for (const auto& entry : docCount) {
        const std::string& docName = entry.first;
        int count = entry.second;

        if (count == includedWords.size()) {
            for (const auto& word : includedWords) {
                auto results = searchWord(word);
                for (const auto& wid : results) {
                    if (wid->getDocName() == docName) {
                        result.push_back(wid);
                    }
                }
            }
        }
    }

    std::sort(result.begin(), result.end(), [](WordInDocument* a, WordInDocument* b) {
        return a->getPositions().size() > b->getPositions().size();
        });

    return result;
}
