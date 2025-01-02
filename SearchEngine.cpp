#include "SearchEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <unordered_set>
bool SearchEngine::useHashMap = true;

using namespace std;

string SearchEngine::normalize(const string& word) {
    string result;
    for (char c : word) {
        if (isalnum(c)) {
            result += tolower(c);
        }
    }
    return result;
}

vector<string> SearchEngine::splitQuery(const string& query) const {
    vector<string> words;
    string currentWord;
    for (char c : query) {
        if (isspace(c)) {
            if (!currentWord.empty()) {
                words.push_back(currentWord);
                currentWord.clear();
            }
        }
        else {
            currentWord += c;
        }
    }
    if (!currentWord.empty()) {
        words.push_back(currentWord);
    }
    return words;
}

void SearchEngine::displayResults(const vector<WordInDocument*>& results) const {
    if (results.empty()) {
        cout << "No results found.\n";
        return;
    }
    for (const auto& result : results) {
        result->display();
    }
}

void SearchEngine::indexDocuments(const string& folderPath, bool useHashMap, int numFiles) {
    int filesIndexed = 0;
    for (int i = 1; filesIndexed < numFiles; ++i) {
        ostringstream fileName;
        fileName << folderPath << "/review_" << i << ".txt";
        if (fs::exists(fileName.str())) {
            indexDocument(fileName.str(), useHashMap);
            filesIndexed++;
        }
        else {
            cout << "File not found: " << fileName.str() << "\n";
        }
    }
    cout << "Indexed " << filesIndexed << " files successfully.\n";
}

void SearchEngine::indexDocument(const string& filePath, bool useHashMap) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filePath << "\n";
        return;
    }

    static unordered_set<string> indexedFiles;
    if (indexedFiles.find(filePath) != indexedFiles.end()) {
        cout << "File already indexed: " << filePath << "\n";
        return;
    }
    indexedFiles.insert(filePath);

    string word;
    int position = 0;
    while (file >> word) {
        string normalizedWord = normalize(word);
        if (!normalizedWord.empty()) {
            if (useHashMap) {
                hashMapSearch.insertWord(normalizedWord, filePath, position++);
            }
            else {
                trieSearch.insertWord(normalizedWord, filePath, position++);
            }
        }
    }

    file.close();
    cout << "File indexed successfully: " << filePath << "\n";
}

void SearchEngine::searchQuery(const string& query, bool useHashMap) const {
    vector<string> words = splitQuery(query);  
    vector<WordInDocument*> results;

    if (query.front() == '"' && query.back() == '"') {
        string phrase = query.substr(1, query.size() - 2);  // Remove quotes
        vector<string> phraseWords = splitQuery(phrase);
        results = useHashMap ? hashMapSearch.searchMultipleWords(phraseWords) : trieSearch.searchMultipleWords(phraseWords);
    }

    else if (query.find("+") != string::npos) {
        vector<string> includedWords, excludedWords;
        bool isPlusQuery = true;
        for (const auto& word : words) {
            if (word == "+") {
                isPlusQuery = false;
                continue;
            }
            if (isPlusQuery) {
                includedWords.push_back(word);
            }
            else {
                excludedWords.push_back(word);
            }
        }
        if (excludedWords.empty()) {
            results = useHashMap ? hashMapSearch.searchMultipleWords(includedWords) : trieSearch.searchMultipleWords(includedWords);
        }
        else {
            results = useHashMap ? hashMapSearch.searchExclusion(includedWords[0], excludedWords[0]) : trieSearch.searchExclusion(includedWords[0], excludedWords[0]);
        }
    }

    else if (query.find("-") != string::npos) {
        if (words.size() >= 3 && words[1] == "-") {
            string word1 = words[0];
            string word2 = words[2];
            results = useHashMap ? hashMapSearch.searchExclusion(word1, word2) : trieSearch.searchExclusion(word1, word2);
        }
    }

    else if (words.size() == 1) {
        results = useHashMap ? hashMapSearch.searchWord(words[0]) : trieSearch.searchWord(words[0]);
    }

    else {
        results = useHashMap ? hashMapSearch.searchMultipleWords(words) : trieSearch.searchMultipleWords(words);
    }


    sort(results.begin(), results.end(), [](WordInDocument* a, WordInDocument* b) {

        if (a->getFrequency() != b->getFrequency()) {
            return a->getFrequency() > b->getFrequency();  
        }
        return a->getDocName() < b->getDocName();  
        });

    displayResults(results);  
}


void SearchEngine::comparePerformance(const vector<string>& queries) {
    auto start = chrono::high_resolution_clock::now();
    for (const auto& query : queries) {
        hashMapSearch.searchWord(query);
    }
    auto end = chrono::high_resolution_clock::now();
    auto hashMapDuration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    start = chrono::high_resolution_clock::now();
    for (const auto& query : queries) {
        trieSearch.searchWord(query);
    }
    end = chrono::high_resolution_clock::now();
    auto trieDuration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "HashMap Search Time: " << hashMapDuration << " ms\n";
    cout << "Trie Search Time: " << trieDuration << " ms\n";
}

void SearchEngine::clear(bool useHashMap) {
    if (useHashMap) {
        hashMapSearch.clear();
    }
    else {
        trieSearch.clear();
    }
}

bool SearchEngine::dumpSearchEngine(const string& dumpFilePath) {
    ofstream dumpFile(dumpFilePath, ios::binary);
    if (!dumpFile.is_open()) {
        cerr << "Error opening dump file for writing: " << dumpFilePath << "\n";
        return false;
    }
    if (useHashMap) {
        hashMapSearch.save(dumpFile);
    }
    else {
        trieSearch.save(dumpFile);
    }
    dumpFile.close();
    cout << "Search engine index dumped successfully to " << dumpFilePath << "\n";
    return true;
}

bool SearchEngine::loadSearchEngine(const string& dumpFilePath) {
    ifstream dumpFile(dumpFilePath, ios::binary);
    if (!dumpFile.is_open()) {
        cerr << "Error opening dump file for reading: " << dumpFilePath << "\n";
        return false;
    }

    if (useHashMap) {
        hashMapSearch.clear();
        hashMapSearch.load(dumpFile);
    }
    else {
        trieSearch.clear();
        trieSearch.load(dumpFile);
    }

    dumpFile.close();
    cout << "Search engine index loaded successfully from " << dumpFilePath << "\n";
    return true;
}


void SearchEngine::addFileToIndex(const string& newFilePath, bool useHashMap) {
    indexDocument(newFilePath, useHashMap);
}
