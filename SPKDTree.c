/*
 * SPKDTree.c
 *
 *  Created on: Jul 19, 2016
 *      Author: mataneilat
 */

#include "SPKDTree.h"
#include <stdlib.h>
#include <math.h>

/*** Type Declarations ***/

struct sp_kd_tree_node_t {
	int dim;
	double medianVal;
	struct sp_kd_tree_node_t *leftChild;
	struct sp_kd_tree_node_t *rightChild;
	SPPoint *data;
};

/*** Private Methods ***/

/**
 * Recursive implementation of spKDTreeBuild.
 *
 * @param kdArray The kd-array used to build the tree
 * @param splitMethod The method which determines the dimension to split the array by.
 * @param previousSplitDimension The dimension that the array was previously split by (in the "upper level")- for INCREMENTAL split method.
 *
 * @return
 * 	NULL in case the given tree is NULL, or an allocation failure occurred
 * 	Otherwise, return the root of the newly create tree.
 */
SPKDTreeNode buildTree(SPKDArray kdArray, SP_TREE_SPLIT_METHOD splitMethod, int previousSplitDimension) {
	int arraySize, splitDimension, maxDimension;
	SPKDTreeNode treeNode = NULL;
	SPKDArraySplitResult splitResult = NULL;
	SPPoint *data;
	if (kdArray == NULL) {
		return NULL;
	}
	arraySize = spKDArrayGetSize(kdArray);
	if (arraySize == 0) {
		return NULL;
	}
	treeNode = (SPKDTreeNode) malloc(sizeof(*treeNode));
	if (treeNode == NULL) {
		return NULL;
	}
	if (arraySize == 1) {
		data = (SPPoint *) malloc(sizeof(SPPoint));
		if (data == NULL) {
			free(treeNode);
			return NULL;
		}
		data = spKDArrayGetPointsArrayCopy(kdArray);

		treeNode->dim = -1;
		treeNode->medianVal = INFINITY;
		treeNode->leftChild = NULL;
		treeNode->rightChild = NULL;
		treeNode->data = data;
		return treeNode;
	}
	maxDimension = spKDArrayGetPointsDimension(kdArray);
	switch (splitMethod) {
	case TREE_SPLIT_METHOD_MAX_SPREAD:
		splitDimension = spKDArrayMaxSpreadDimension(kdArray);
		break;
	case TREE_SPLIT_METHOD_RANDOM:
		splitDimension = rand() % maxDimension;
		break;
	case TREE_SPLIT_METHOD_INCREMENTAL:
		splitDimension = (previousSplitDimension + 1) % maxDimension;
		break;
	}
	splitResult = spKDArraySplit(kdArray, splitDimension);
	if (splitResult == NULL) {
		free(treeNode);
		return NULL;
	}

	treeNode->dim = splitDimension;
	treeNode->medianVal = spKDArrayGetMedian(kdArray, splitDimension);
	treeNode->leftChild = buildTree(spKDArraySplitResultGetLeft(splitResult), splitMethod, splitDimension);
	treeNode->rightChild = buildTree(spKDArraySplitResultGetRight(splitResult), splitMethod, splitDimension);
	treeNode->data = NULL;
	// The left are right arrays are used, so we do not free them
	spKDArraySplitResultDestroy(splitResult);
	return treeNode;
}

/*** Public Methods ***/

SPKDTreeNode spKDTreeBuild(SPKDArray kdArray, SP_TREE_SPLIT_METHOD splitMethod) {
	return buildTree(kdArray, splitMethod, -1);
}

void spKDTreeDestroy(SPKDTreeNode treeNode) {
	if (treeNode == NULL) return;
	if (treeNode->data != NULL) {
		spPointDestroy(*(treeNode->data));
	}
	spKDTreeDestroy(treeNode->leftChild);
	spKDTreeDestroy(treeNode->rightChild);
	free(treeNode);
}

bool spKDTreeNodeIsLeaf(SPKDTreeNode treeNode) {
	if (treeNode == NULL) {
		return false;
	}
	return (treeNode->dim == -1 && treeNode->medianVal == INFINITY && treeNode->rightChild == NULL && treeNode->leftChild == NULL);
}

int spKDTreeNodeGetDimension(SPKDTreeNode treeNode) {
	if (treeNode == NULL) return -1;
	return treeNode->dim;
}

double spKDTreeNodeGetMedianValue(SPKDTreeNode treeNode) {
	if (treeNode == NULL) return INFINITY;
	return treeNode->medianVal;
}

SPKDTreeNode spKDTreeNodeGetLeftChild(SPKDTreeNode treeNode) {
	if (treeNode == NULL) return NULL;
	return treeNode->leftChild;
}

SPKDTreeNode spKDTreeNodeGetRightChild(SPKDTreeNode treeNode) {
	if (treeNode == NULL) return NULL;
	return treeNode->rightChild;
}

SPPoint *spKDTreeNodeGetData(SPKDTreeNode treeNode) {
	if (treeNode == NULL) return NULL;
	return treeNode->data;
}

