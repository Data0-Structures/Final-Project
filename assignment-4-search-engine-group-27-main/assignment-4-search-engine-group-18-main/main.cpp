#include <iostream>
#include "document_parser.h"
#include <vector>
#include <algorithm>
#include <set>
#include <chrono>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace std::chrono;

void clearIndexFiles() {
    std::vector<std::string> indexFiles = {"mainIndex.txt", "organizationIndex.txt", "personIndex.txt"};

    for (const auto& file : indexFiles) {
        // Check if the file exists, and if so, remove it
        if (std::filesystem::exists(file)) {
            std::filesystem::remove(file);
        }
    }
}


void printMergedSets(const set<string>& uniqueSet, const set<string>& uniqueSetOrg, const set<string>& uniqueSetPerson) {
    // Merge the sets
    set<string> mergedSet;
    mergedSet.insert(uniqueSet.begin(), uniqueSet.end());
    mergedSet.insert(uniqueSetOrg.begin(), uniqueSetOrg.end());
    mergedSet.insert(uniqueSetPerson.begin(), uniqueSetPerson.end());

    // Count the frequency of each string in the merged set
    vector<pair<string, int>> frequencyVector;
    for (const auto& str : mergedSet) {
        int frequency = 0;
        frequency += uniqueSet.count(str);
        frequency += uniqueSetOrg.count(str);
        frequency += uniqueSetPerson.count(str);
        frequencyVector.emplace_back(str, frequency);
    }

    // Sort the vector based on frequencies in descending order
    sort(frequencyVector.begin(), frequencyVector.end(),
         [](const auto& a, const auto& b) {
             return a.second > b.second;
         });

    // Print the sorted container
    for (const auto& [str, frequency] : frequencyVector) {
        cout << str << ": " << frequency << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <search_query>" << endl;
        return 1;
    }

    AVLTree mainIndex;  // Create an instance of AVLTree
    AVLTree organizationIndex;
    AVLTree personsIndex;

    clearIndexFiles();//clear content then reload
    // Gather stats
    auto indexingStart = high_resolution_clock::now();

    DocumentParser::readFileSystem("sample_data", mainIndex, organizationIndex, personsIndex); //Loading data from the dataset
    vector<string> searchTerms;
    vector<string> excludeTerms;

    // Separate inclusion and exclusion terms
    for (int i = 1; i < argc; ++i) {
        string term(argv[i]);
        transform(term.begin(), term.end(), term.begin(), ::tolower);  // Convert to lowercase

        if (term.find("-") == 0) {
            // Exclude term
            excludeTerms.push_back(term.substr(1));
        } else {
            // Include term
            searchTerms.push_back(term);
        }
    }

    set<string> uniqueSet;
    set<string> uniqueSetOrg;
    set<string> uniqueSetPerson;

    set<string> uniqueSetExclude;

    for (const auto& term : searchTerms) {
        vector<AVLNode*> termResult;  // Results for the current term

        if (term.find("org:") == 0) {
            organizationIndex.loadFromFile("organizationIndex.txt");
            std::string orgTerm = term.substr(4);  // Extract organization name
            orgTerm = DocumentParser::cleanText(orgTerm);
            termResult = organizationIndex.search(orgTerm);

            for (const auto& node : termResult) {
                vector<string> documents = node->documents;
                for(const auto& document : documents) {
                    uniqueSetOrg.insert(document);
                }
            }
        } else if (term.find("person:") == 0) {
            personsIndex.loadFromFile("personIndex.txt");
            std::string personTerm = term.substr(7);  // Extract person name
            personTerm = DocumentParser::cleanText(personTerm);;
            termResult = personsIndex.search(personTerm);

            for (const auto& node : termResult) {
                vector<string> documents = node->documents;
                for(const auto& document : documents) {
                    uniqueSetPerson.insert(document);
                }
            }
        } else {
            mainIndex.loadFromFile("mainIndex.txt");
            std::string termSearch = term;
            std::string wordSearch = DocumentParser::cleanText(termSearch);
            termResult = mainIndex.search(wordSearch);
            for (const auto& node : termResult) {
                vector<string> documents = node->documents;
                for(const auto& document : documents) {
                    uniqueSet.insert(document);
                }
            }
        }
    }

    for (const auto& term : excludeTerms) {
        vector<AVLNode*> termResult;
        if (term.find("org:") == 0) {
            organizationIndex.loadFromFile("organizationIndex.txt");
            std::string orgTerm = term.substr(4);  // Extract organization name
            orgTerm = DocumentParser::cleanText(orgTerm);
            termResult = organizationIndex.search(orgTerm);

            for (const auto& node : termResult) {
                vector<string> documents = node->documents;
                for(const auto& document : documents) {
                    uniqueSetOrg.erase(document);
                    uniqueSet.erase(document);
                    uniqueSetPerson.erase(document);
                }
            }
        } else if (term.find("person:") == 0) {
            personsIndex.loadFromFile("personIndex.txt");
            std::string personTerm = term.substr(7);  // Extract person name
            personTerm = DocumentParser::cleanText(personTerm);;
            termResult = personsIndex.search(personTerm);

            for (const auto& node : termResult) {
                vector<string> documents = node->documents;
                for(const auto& document : documents) {
                    uniqueSetOrg.erase(document);
                    uniqueSet.erase(document);
                    uniqueSetPerson.erase(document);
                }
            }
        } else {
            mainIndex.loadFromFile("mainIndex.txt");
            std::string termSearch = term;
            std::string wordSearch = DocumentParser::cleanText(termSearch);
            termResult = mainIndex.search(wordSearch);
            for (const auto& node : termResult) {
                vector<string> documents = node->documents;
                for(const auto& document : documents) {
                    uniqueSetOrg.erase(document);
                    uniqueSet.erase(document);
                    uniqueSetPerson.erase(document);
                }
            }
        }
    }

    auto indexingStop = high_resolution_clock::now();
    auto indexingDuration = duration_cast<milliseconds>(indexingStop - indexingStart);


    cout << "Sorted Result :" << endl;
    printMergedSets(uniqueSet, uniqueSetOrg, uniqueSetPerson);

    // Save stats to a file
    ofstream statsFile("stats.txt");
    if (statsFile.is_open()) {
        statsFile << "Indexing Time: " << indexingDuration.count() << " ms\n";// Output indexing time
        statsFile << "Total Number of Individual Articles: " << DocumentParser::getTotalArticlesProcessed()<< "\n";// Output total number of individual articles
        statsFile << "Total Number of Unique Words Indexed: " << DocumentParser::getTotalUniqueWordsIndexed() << "\n";// Output total number of unique words indexed
        statsFile.close();
    } else {
        cerr << "Error opening stats file for writing." << endl;
    }


    return 0;
}
