/**
 * Example code related to final project
 */

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <codecvt>
#include <sstream>
#include <iterator>

// RapidJSON headers we need for our parsing.
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"
#include "english_stem.h"
#include "document_parser.h"


using namespace rapidjson;
using namespace std;

int DocumentParser::totalArticlesProcessed = 0;
int DocumentParser::totalUniqueWordsIndexed = 0;
// // Function Prototypes
void readJsonFiles(const string &fileName, AVLTree &mainIndex, AVLTree &organizationIndex, AVLTree &personsIndex);
void readFileSystem(const string &path, AVLTree &mainIndex, AVLTree &organizationIndex, AVLTree &personsIndex);
std::string cleanText(string &word);
void loadMainIndex(const string &fileName, AVLTree &mainIndex);
void loadOrganizationIndex(const string &fileName, AVLTree &organizationIndex);
void loadPersonsIndex(const string &fileName, AVLTree &personsIndex);



std::string DocumentParser::applyStemming(const std::string& inputText) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring stemmedText = converter.from_bytes(inputText);

    stemming::english_stem<> StemEnglish;

    // Tokenize the input text into words
    std::wstringstream wss(stemmedText);
    std::vector<std::wstring> words{ std::istream_iterator<std::wstring, wchar_t>(wss),
                                 std::istream_iterator<std::wstring, wchar_t>() };

    // Apply stemming to each word
    for (auto& word : words) {
        StemEnglish(word);
    }

    // Recreate the string with stemmed words
    std::wstringstream result;
    std::copy(words.begin(), words.end(), std::ostream_iterator<std::wstring, wchar_t>(result, L" "));

    return converter.to_bytes(result.str());
}

std::string DocumentParser::removePunctuation(const std::string& inputText) {
    std::string result;

    for (char ch : inputText) {
        // Check if the character is not a punctuation mark
        if (!std::ispunct(ch)) {
            result += ch;
        }
    }

    return result;
}

std::string DocumentParser::removeStopWords(const std::string& inputText, const std::string& stopWordsFile) {
    // Read stop words from the file into a vector
    std::ifstream stopWordsStream(stopWordsFile);
    if (!stopWordsStream.is_open()) {
        std::cerr << "Error opening stop words file." << std::endl;
        return inputText;  // Return the input text unchanged in case of an error
    }

   std::vector<std::string> stopWords;
    std::string stopWord;
    while (stopWordsStream >> stopWord) {
        // Convert stop word to lowercase for case-insensitive comparison
        std::transform(stopWord.begin(), stopWord.end(), stopWord.begin(), ::tolower);
        stopWords.push_back(stopWord);
    }
    // Close the stop words file
    stopWordsStream.close();

    
    // Tokenize the input text into words
    std::istringstream iss(inputText);
    std::vector<std::string> words(std::istream_iterator<std::string>{iss},
                                   std::istream_iterator<std::string>());

    // Remove stop words
    words.erase(std::remove_if(words.begin(), words.end(), [&stopWords](const std::string& word) {
        std::string lowerWord = word;
        std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
        bool isStopWord = std::find(stopWords.begin(), stopWords.end(), lowerWord) != stopWords.end();
        return isStopWord;
    }), words.end());

    // Recreate the string without stop words
    std::ostringstream result;
    std::copy(words.begin(), words.end(), std::ostream_iterator<std::string>(result, " "));

    return result.str();
}

std::string DocumentParser::cleanText(std::string& inputText) { 
    inputText = removePunctuation(inputText);
   inputText = removeStopWords(inputText,"stop_words_english.txt");
    inputText = applyStemming(inputText);

    return inputText;
}

/**
 * example code that reads and parses a json file and extracts the title and person
 * entities.
 * @param fileName filename with relative or absolute path included.
 */
void DocumentParser::readJsonFiles(const string &fileName, AVLTree &mainIndex, AVLTree &organizationIndex, AVLTree &personsIndex)
{

    // open an ifstream on the file of interest and check that it could be opened.
    loadMainIndex(fileName, mainIndex);
    loadOrganizationIndex(fileName, organizationIndex);
    loadPersonsIndex(fileName, personsIndex);
}

void DocumentParser::loadMainIndex(const string &fileName, AVLTree &mainIndex) {
 ifstream input(fileName);
    if (!input.is_open())
    {
        cerr << "cannot open file: " << fileName << endl;
        return;
    }

    // Create a RapidJSON IStreamWrapper using the file input stream above.
    IStreamWrapper isw(input);

    // Create a RapidJSON Document object and use it to parse the IStreamWrapper object above.
    Document d;
    d.ParseStream(isw);

    // Now that the document is parsed, we can access different elements the JSON using
    // familiar subscript notation.

    // This accesses the -text- element in the JSON. Since the value
    //  associated with text is a string (rather than
    //  an array or something else), we call the GetString()
    //  function to return the actual text of the article
    //  as a c-string.
    auto val = d["text"].GetString();    
    
    // auto valWithoutStopWords = removeStopWords(val,"stop_words_english.txt");//Remove all the stop words
    // auto valNewPunct = removePunctuation(valWithoutStopWords);//Remove punctuation marks
    // auto stemmedWords = applyStemming(valNewPunct);//Stem the text
    // cout << "Text without stop words is: " << stemmedWords << "\n";
    std::string textToClean = d["text"].GetString();
    std::string cleaned = cleanText(textToClean);

    std::istringstream iss(cleaned);
    std::string word;
    while (iss >> word) {
        mainIndex.insert(word, fileName);//insert into the index
        DocumentParser::totalUniqueWordsIndexed++;
    }
    mainIndex.saveToFile("mainIndex.txt");
    mainIndex.clear();
}

void DocumentParser::loadOrganizationIndex(const string &fileName, AVLTree &organizationIndex) {
 ifstream input(fileName);
    if (!input.is_open())
    {
        cerr << "cannot open file: " << fileName << endl;
        return;
    }

    // Create a RapidJSON IStreamWrapper using the file input stream above.
    IStreamWrapper isw(input);

    // Create a RapidJSON Document object and use it to parse the IStreamWrapper object above.
    Document d;
    d.ParseStream(isw);

    // Now that the document is parsed, we can access different elements the JSON using
    // familiar subscript notation.

    // This accesses the -text- element in the JSON. Since the value
    //  associated with text is a string (rather than
    //  an array or something else), we call the GetString()
    //  function to return the actual text of the article
    //  as a c-string.
     auto orgs = d["entities"]["organizations"].GetArray();

    // We iterate over the Array returned from the line above.
    //  Each element kind of operates like a little JSON document
    //  object in that you can use the same subscript notation
    //  to access particular values.
    for (auto &o : orgs)
    {
        // cout << "    > " << setw(30) << left << o["name"].GetString()
        //      << setw(10) << left << o["sentiment"].GetString() << "\n";
        std::string textToClean = o["name"].GetString();
        std::string cleaned = cleanText(textToClean);
        std::istringstream iss(cleaned);
        std::string word;
        while (iss >> word) {
            organizationIndex.insert(word, fileName);
            DocumentParser::totalUniqueWordsIndexed++;
        }
        // organizationIndex.insert(cleaned, fileName);     
    }

    organizationIndex.saveToFile("organizationIndex.txt");
    organizationIndex.clear();
}

void DocumentParser::loadPersonsIndex(const string &fileName, AVLTree &personsIndex) {
 ifstream input(fileName);
    if (!input.is_open())
    {
        cerr << "cannot open file: " << fileName << endl;
        return;
    }

    // Create a RapidJSON IStreamWrapper using the file input stream above.
    IStreamWrapper isw(input);

    // Create a RapidJSON Document object and use it to parse the IStreamWrapper object above.
    Document d;
    d.ParseStream(isw);

    // Now that the document is parsed, we can access different elements the JSON using
    // familiar subscript notation.

    // This accesses the -text- element in the JSON. Since the value
    //  associated with text is a string (rather than
    //  an array or something else), we call the GetString()
    //  function to return the actual text of the article
    //  as a c-string.
     auto orgs = d["entities"]["persons"].GetArray();

    // We iterate over the Array returned from the line above.
    //  Each element kind of operates like a little JSON document
    //  object in that you can use the same subscript notation
    //  to access particular values.
    for (auto &o : orgs)
    {
        // cout << "    > " << setw(30) << left << o["name"].GetString()
        //      << setw(10) << left << o["sentiment"].GetString() << "\n";
        std::string textToClean = o["name"].GetString();
        std::string cleaned = cleanText(textToClean);
        std::istringstream iss(cleaned);
        std::string word;
        while (iss >> word) {
            personsIndex.insert(word, fileName);
            DocumentParser::totalUniqueWordsIndexed++;
        }
        // personsIndex.insert(cleaned, fileName);     
    }

    personsIndex.saveToFile("personIndex.txt");
    personsIndex.clear();
}

/**
 * example code for how to traverse the filesystem using std::filesystem
 * which is new for C++17.
 *
 * @param path an absolute or relative path to a folder containing files
 * you want to parse.
 */
void DocumentParser::readFileSystem(const string &path, AVLTree &mainIndex, AVLTree &organizationIndex, AVLTree &personsIndex)
{

    // recursive_director_iterator used to "access" folder at parameter -path-
    // we are using the recursive iterator so it will go into subfolders.
    // see: https://en.cppreference.com/w/cpp/filesystem/recursive_directory_iterator
    auto it = filesystem::recursive_directory_iterator(path);

    // loop over all the entries.
    for (const auto &entry : it)
    {

        // cout << "--- " << setw(60) << left << entry.path().c_str() << " ---" << endl;

        // We only want to attempt to parse files that end with .json...
        if (entry.is_regular_file() && entry.path().extension().string() == ".json")
        {
            readJsonFiles(entry.path().string(), mainIndex, organizationIndex, personsIndex);
            DocumentParser::totalArticlesProcessed++;
        }
    }
}

