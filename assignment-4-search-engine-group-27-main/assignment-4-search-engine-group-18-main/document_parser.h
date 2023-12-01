#ifndef DOCUMENT_PARSER_H
#define DOCUMENT_PARSER_H

#include <string>
#include "AVLTree.h"

class DocumentParser {
private:
    static int totalArticlesProcessed;
    static int totalUniqueWordsIndexed;
public:
    static void readJsonFiles(const std::string &fileName, AVLTree &mainIndex, AVLTree &organizationIndex, AVLTree &personsIndex);
    static void loadMainIndex(const std::string &fileName, AVLTree &mainIndex);
    static void loadOrganizationIndex(const std::string &fileName, AVLTree &organizationIndex);
    static void loadPersonsIndex(const std::string &fileName, AVLTree &personsIndex);

    static std::string applyStemming(const std::string& inputText);
    static std::string removePunctuation(const std::string& inputText);
    static std::string removeStopWords(const std::string& inputText, const std::string& stopWordsFile);
    static std::string cleanText(std::string& inputText);

    static void readFileSystem(const std::string &path, AVLTree &mainIndex, AVLTree &organizationIndex, AVLTree &personsIndex);

    static int getTotalArticlesProcessed() {
        return totalArticlesProcessed;
    }

    static int getTotalUniqueWordsIndexed() {
        return totalUniqueWordsIndexed;
    }
};

#endif // DOCUMENT_PARSER_H
