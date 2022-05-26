#ifndef BINARY_TREE
#define BINARY_TREE

#include "../../../Curve-Modules/Curve/Curve.h"
#include <vector>

class BinaryTree {

private:
    Curve **tree;
    unsigned int size;
    unsigned int vectorSpaceDimensions;

    Curve *getCurve(unsigned int);
    unsigned int getLeft(unsigned int);
    unsigned int getRight(unsigned int);
    void setCurve(unsigned int, Curve *);
    bool IsLeaf(unsigned int);
    Curve *calculateMeanCurve(Curve *, Curve *);
    Curve *postOrderTraversal(unsigned int);
    BinaryTree(unsigned int, unsigned int);

public:
    static BinaryTree *createBinaryTree(unsigned int, unsigned int);
    void scatterCurvesToLeaves(std::vector<Curve *> &);
    Curve *postOrderTraversal();
    ~BinaryTree();
};


#endif
