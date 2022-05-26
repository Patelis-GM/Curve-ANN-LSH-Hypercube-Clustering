#include "CurveCluster.h"
#include "../Binary-Tree/BinaryTree.h"

CurveCluster::CurveCluster(Curve *centroid) {
    this->centroid = centroid;
    this->shouldDeleteCentroid = false;
}

Curve *CurveCluster::getCentroid() {
    return this->centroid;
}

const std::vector<Curve *> *CurveCluster::getAssignedCurves() const {
    return &this->assignedCurves;
}

void CurveCluster::updateCentroid(std::vector<Curve *> &curves, unsigned int vectorSpaceDimensions) {

    /* If the previous Centroid is not a Curve within the dataset, delete it */
    if (shouldDeleteCentroid)
        delete this->centroid;

    /* If there's only 1 Curve within the Cluster, set it directly as the new Centroid */
    if (curves.size() == 1) {
        this->centroid = curves[0];
        this->shouldDeleteCentroid = false;
    }

    else {

        /* Otherwise, create a complete binary tree and scatter the Curves to its leaves */
        BinaryTree *binaryTree = BinaryTree::createBinaryTree(curves.size(), vectorSpaceDimensions);
        binaryTree->scatterCurvesToLeaves(curves);

        /* Calculate the mean Curve and set it as the new Centroid */
        this->centroid = binaryTree->postOrderTraversal();
        this->shouldDeleteCentroid = true;
        delete binaryTree;
    }
}

void CurveCluster::assignCurve(Curve *assignedCurve) {
    this->assignedCurves.push_back(assignedCurve);
}

CurveCluster::~CurveCluster() {

    if (this->shouldDeleteCentroid)
        delete this->centroid;
}

