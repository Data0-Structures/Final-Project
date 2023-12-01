// avltree.cpp
#include "AVLTree.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

AVLTree::~AVLTree() {
    // Make sure to call clear in the destructor to free allocated memory
    clear(root);
}

void AVLTree::clear() {
    // Call the recursive clear function starting from the root
    clear(root);
    // Set the root to nullptr after clearing
    root = nullptr;
}

void AVLTree::clear(AVLNode* node) {
    if (node != nullptr) {
        // Recursively clear the left and right subtrees
        clear(node->left);
        clear(node->right);

        // Delete the current node
        delete node;
    }
}

AVLNode::AVLNode(const std::string& word, const std::string& document)
    : key(word),frequency(1), height(1), left(nullptr), right(nullptr) {
    documents.push_back(document);
}

AVLTree::AVLTree() : root(nullptr) {}

int AVLTree::getHeight(AVLNode* node) {
    return (node == nullptr) ? 0 : node->height;
}

int AVLTree::getBalanceFactor(AVLNode* node) {
    return (node == nullptr) ? 0 : getHeight(node->left) - getHeight(node->right);
}

AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));
    x->height = 1 + std::max(getHeight(x->left), getHeight(x->right));

    return x;
}

AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + std::max(getHeight(x->left), getHeight(x->right));
    y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));

    return y;
}

AVLNode* AVLTree::balanceNode(AVLNode* node) {
    if (node == nullptr)
        return nullptr;

    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));

    int balance = getBalanceFactor(node);

    // Left Heavy
    if (balance > 1) {
        // Left Right Case
        if (getBalanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
        }
        return rotateRight(node);
    }
    // Right Heavy
    else if (balance < -1) {
        // Right Left Case
        if (getBalanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
        }
        return rotateLeft(node);
    }

    return node;
}

AVLNode* AVLTree::insertNode(AVLNode* node, const std::string& word, const std::string& document) {
    if (node == nullptr)
        return new AVLNode(word, document);

    if (word < node->key)
        node->left = insertNode(node->left, word, document);
    else if (word > node->key)
        node->right = insertNode(node->right, word, document);
    else {
        node->frequency++;
        auto it = std::find(node->documents.begin(), node->documents.end(), document);
        if (it == node->documents.end()) {
            node->documents.push_back(document);
        }
        // node->documents.push_back(document); // Word already exists, add the document
    }

    return balanceNode(node);
}

void AVLTree::insert(const std::string& word, const std::string& document) {
    root = insertNode(root, word, document);
}

void inOrderTraversalPrint(AVLNode* node) {
    if (node != nullptr) {
        inOrderTraversalPrint(node->left);
        std::cout << node->key <<", Freq ="<< node->frequency << "";
        if (!node->documents.empty()) {
            auto it = node->documents.begin();
            std::cout << *it;
            ++it;

            for (; it != node->documents.end(); ++it) {
                std::cout << ", " << *it;
            }
        }
        std::cout << std::endl;
        inOrderTraversalPrint(node->right);
    }
}

void AVLTree::printInOrder() {
    inOrderTraversalPrint(root);
}

void AVLTree::inOrderTraversal(AVLNode* node, std::ofstream& outFile) {
    if (node != nullptr) {
        inOrderTraversal(node->left, outFile);

        // Write node data to the file
        outFile << node->key << "," << node->frequency << ",";
        for (auto it = node->documents.begin(); it != node->documents.end(); ++it) {
            outFile << *it;
            if (std::next(it) != node->documents.end()) {
                outFile << ",";
            }
        }
        outFile << std::endl;
        inOrderTraversal(node->right, outFile);
    }
}


void AVLTree::saveToFile(const std::string& fileName) {
    std::ofstream outFile(fileName, std::ios_base::app);

    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing: " << fileName << std::endl;
        return;
    }

    inOrderTraversal(root, outFile);

    outFile.close();
}

std::vector<std::string> tokenize(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        iss.ignore();
        tokens.push_back(token);
    }
    return tokens;
}

void AVLTree::loadFromFile(const std::string& fileName) {
    // Clear the existing tree
    clear();

    std::ifstream inFile(fileName);

    if (!inFile.is_open()) {
        std::cerr << "Error opening file for reading: " << fileName << std::endl;
        return;
    }

    std::string word;
    int frequency; // To store the frequency of the word
    std::string document;

    while (std::getline(inFile, word, ',') >> frequency) {
        inFile.ignore();

        std::string line;
        std::getline(inFile >> std::ws, line);

        std::vector<std::string> tokens = tokenize(line);

        for (const auto& token : tokens) {
            insert(word,token);
        }

    }

    inFile.close();
}

std::vector<AVLNode*> AVLTree::search(const std::string& word) {
    std::vector<std::string> tokens = tokenize(word);
    std::vector<AVLNode*> result;
        for (const auto& token : tokens) {
            std::vector<AVLNode*> tokenResult = searchNode(root, token);
            result.insert(result.end(), tokenResult.begin(), tokenResult.end());
        }

    return result;  
}

std::vector<AVLNode*> AVLTree::searchNode(AVLNode* node, const std::string& word) {
    std::vector<AVLNode*> result;

    if (node == nullptr) {
        // Word not found
        return result;
    }

    if (word == node->key) {
        // Word found in the current node
        result.push_back(node);
    }

    // Search in the left subtree
    std::vector<AVLNode*> leftResult = searchNode(node->left, word);
    result.insert(result.end(), leftResult.begin(), leftResult.end());

    // Search in the right subtree
    std::vector<AVLNode*> rightResult = searchNode(node->right, word);
    result.insert(result.end(), rightResult.begin(), rightResult.end());

    return result;
}