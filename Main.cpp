#include "SearchEngine.h"
#include <iostream>
#include <string>
#include <vector>
#include <experimental/filesystem>
#include <fstream>

namespace fs = std::experimental::filesystem;
using namespace std;

void printMenu() {
    cout << "\n========== SEARCH ENGINE ==========\n";
    cout << "1. Index documents from a folder\n";
    cout << "2. Search for a word/phrase\n";
    cout << "3. Search for multiple words\n";
    cout << "4. Add a new file to the index\n";
    cout << "5. Dump the search engine to a file\n";
    cout << "6. Load the search engine from a file\n";
    cout << "7. Compare HashMap and Trie performance\n";
    cout << "0. Exit\n";
    cout << "===================================\n";
    cout << "Enter your choice: ";
}

void clearScreen() {
    system("cls");
}

int main() {
    SearchEngine searchEngine;
    string query;
    int choice;

    const string folderPath = "C:/Users/chaud/source/repos/Search Engine/review_text/review_text";
    const string dumpFilePath = "search_engine.dat";

    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        cerr << "Error: Folder path does not exist or is not a directory. Exiting program." << endl;
        return 1;
    }

    cout << "Welcome to the Search Engine!" << endl;
    cout << "Choose the underlying data structure:\n";
    cout << "1. HashMap\n";
    cout << "2. Trie\n";
    cout << "Enter your choice: ";
    cin >> choice;
    SearchEngine::useHashMap = (choice == 1);
    clearScreen();

    do {
        printMenu();
        cin >> choice;
        switch (choice) {
        case 1: {
            int numFiles;
            cout << "Enter the number of files to index: ";
            cin >> numFiles;
            if (numFiles <= 0) {
                cerr << "Invalid number of files. Please enter a positive number." << endl;
                break;
            }
            searchEngine.indexDocuments(folderPath, SearchEngine::useHashMap, numFiles);
            cout << "Indexed " << numFiles << " files successfully from folder: " << folderPath << endl;
            break;
        }
        case 2: {
            cout << "Enter your search query: ";
            cin.ignore();
            getline(cin, query);
            searchEngine.searchQuery(query, SearchEngine::useHashMap);
            break;
        }
        case 3: {
            cout << "Enter your search query (e.g., word1 +word2 -word3): ";
            cin.ignore();
            getline(cin, query);
            searchEngine.searchQuery(query, SearchEngine::useHashMap);
            break;
        }
        case 4: {
            string newFilePath;
            cout << "Enter the path of the new file to index: ";
            cin.ignore();
            getline(cin, newFilePath);
            if (!fs::exists(newFilePath)) {
                cerr << "Error: File does not exist. Please provide a valid file path." << endl;
                break;
            }
            searchEngine.addFileToIndex(newFilePath, SearchEngine::useHashMap);
            cout << "File indexed successfully: " << newFilePath << endl;
            break;
        }
        case 5: {
            if (searchEngine.dumpSearchEngine(dumpFilePath)) {
                cout << "Search engine data saved to: " << dumpFilePath << endl;
            }
            else {
                cerr << "Error: Could not save the search engine data." << endl;
            }
            break;
        }
        case 6: {
            if (searchEngine.loadSearchEngine(dumpFilePath)) {
                cout << "Search engine data loaded from: " << dumpFilePath << endl;
            }
            else {
                cerr << "Error: Could not load the search engine data." << endl;
            }
            break;
        }
        case 7: {
            vector<string> testQueries;
            string testQuery;
            cout << "Enter test queries (enter 'done' to stop):\n";
            cin.ignore();
            while (true) {
                cout << "> ";
                getline(cin, testQuery);
                if (testQuery == "done") break;
                testQueries.push_back(testQuery);
            }
            searchEngine.comparePerformance(testQueries);
            break;
        }
        case 0: {
            cout << "Exiting the Search Engine. Goodbye!" << endl;
            break;
        }
        default:
            cerr << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 0);

    return 0;
}
