// AVLTree.h
#ifndef AVLTREE_H
#define AVLTREE_H

#include <string>
#include <vector>

struct AVLNode {
    std::string key;
    std::vector<std::string> documents;
    int height;
    int frequency;
    AVLNode* left;
    AVLNode* right;

    AVLNode(const std::string& word, const std::string& document);
};

class AVLTree {
private:
    AVLNode* root;

    int getHeight(AVLNode* node);
    int getBalanceFactor(AVLNode* node);
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* balanceNode(AVLNode* node);
    AVLNode* insertNode(AVLNode* node, const std::string& word, const std::string& document);
    std::vector<AVLNode*> searchNode(AVLNode* node, const std::string& word);
    void clear(AVLNode* node);

public:
    AVLTree();
    void insert(const std::string& word, const std::string& document);
    void printInOrder();
    void inOrderTraversal(AVLNode* node, std::ofstream& outFile);
    void saveToFile(const std::string& fileName);
    void loadFromFile(const std::string& fileName);
    void clear(); // New function to clear the tree
    std::vector<AVLNode*> search(const std::string& word);
    ~AVLTree(); // Destructor to ensure proper cleanup
    // Add other operations as needed
};

#endif // AVLTREE_H
