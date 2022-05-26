#include "BinaryTree.h"
#include "../../../Metric-Modules/Discrete-Frechet/DiscreteFrechet.h"
#include <random>
#include <chrono>
#include <algorithm>
#include <vector>

BinaryTree::BinaryTree(unsigned int totalLeaves, unsigned int vectorSpaceDimensions) {

    this->vectorSpaceDimensions = vectorSpaceDimensions;
    this->size = 2 * totalLeaves - 1;
    this->tree = new Curve *[this->size];
    for (unsigned int i = 0; i < this->size; ++i)
        this->tree[i] = nullptr;
}

/* Utility function to return the index of the left child of the node */
unsigned int BinaryTree::getLeft(unsigned int index) {
    return 2 * index + 1;
}

/* Utility function to return the index of the right child of the node */
unsigned int BinaryTree::getRight(unsigned int index) {
    return 2 * index + 2;
}

/* Utility function to return the address of the Curve of the node */
Curve *BinaryTree::getCurve(unsigned int index) {
    return this->tree[index];
}

/* Utility function to set the node of the Curve to a new address */
void BinaryTree::setCurve(unsigned int index, Curve *curve) {
    this->tree[index] = curve;
}

/* Utility function to check if the given node is considered a tree leaf */
bool BinaryTree::IsLeaf(unsigned int index) {
    return (index >= this->size / 2);
}

/* Utility function to place the curves randomly into the leaves of the tree */
void BinaryTree::scatterCurvesToLeaves(std::vector<Curve *> &curves) {

    unsigned int seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    std::shuffle(curves.begin(), curves.end(), std::default_random_engine(seed));

    for (unsigned int i = this->size / 2; i < this->size; ++i)
        this->tree[i] = curves[i - (size / 2)];
}

/* Utility function to traverse the tree recursively in a post order fashion */
Curve *BinaryTree::postOrderTraversal(unsigned int currentIndex) {

    if (this->IsLeaf(currentIndex))
        return this->getCurve(currentIndex);
    else {

        /* Get the two mean Curves, calculated by the children */
        Curve *leftCurve = this->postOrderTraversal(this->getLeft(currentIndex));
        Curve *rightCurve = this->postOrderTraversal(this->getRight(currentIndex));

        /* Calculate the mean Curve given the two mean Curves */
        Curve *meanCurve = this->calculateMeanCurve(leftCurve, rightCurve);

        /* Store the mean Curve into the complete binary tree */
        this->setCurve(currentIndex, meanCurve);

        return meanCurve;
    }
}

/* Wrapper function */
Curve *BinaryTree::postOrderTraversal() { return this->postOrderTraversal(0); }

/* Utility function to calculate the mean Curve using the Discrete Frechet Optimal Traversal */
Curve *BinaryTree::calculateMeanCurve(Curve *leftCurve, Curve *rightCurve) {

    /* Calculate the Discrete Frechet Optimal Traversal between the left and the right Curve */
    DiscreteFrechet discreteFrechet;
    std::vector<std::pair<Vector *, Vector *>> *optimalTraversal = discreteFrechet.getOptimalTraversal(*leftCurve, *rightCurve);

    Curve *meanCurve = Curve::createCurve(optimalTraversal->at(0).first->getDimensions());

    /* Find the mean Vector of the Vector pair of the Optimal Traversal and assign it to the Mean Curve */
    for (std::pair<Vector *, Vector *> pair: *optimalTraversal) {

        /* If the mean Curve gets too large, stop any further calculations */
        if (meanCurve->getDimensions() * meanCurve->getComplexity() == this->vectorSpaceDimensions)
            break;

        meanCurve->insert(Vector::meanVector({pair.first, pair.second}));
    }

    delete optimalTraversal;

    return meanCurve;
}

BinaryTree::~BinaryTree() {

    for (unsigned int i = 1; i < this->size / 2; ++i)
        delete this->tree[i];

    delete[] this->tree;
}

/* Create a complete binary tree that contains <size> leaves  */
BinaryTree *BinaryTree::createBinaryTree(unsigned int size, unsigned int vectorSpaceDimensions) {

    if (size == 0 || vectorSpaceDimensions == 0)
        return nullptr;

    return new BinaryTree(size, vectorSpaceDimensions);
}

