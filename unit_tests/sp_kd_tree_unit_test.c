/*
 * sp_kd_tree_unit_test.c
 *
 *  Created on: Jul 26, 2016
 *      Author: mataneilat
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>
#include "unit_test_util.h"
#include "common_test_util.h"
#include "../SPKDTree.h"

static bool innerNodeState(SPKDTreeNode treeNode, int expectedDimension, double expectedMedian);
static bool leafNodeState(SPKDTreeNode treeNode, SPPoint expectedData);

static bool kdTreeIncrementalProperBuildTest() {
	SPPoint *points = (SPPoint *) malloc(6 * sizeof(*points));
	points[0] = threeDPoint(1, 60, -5.5);
	points[1] = threeDPoint(2, 80, 4.5);
	points[2] = threeDPoint(9, 140.5, 7.5);
	points[3] = threeDPoint(3, 8, 133.5);
	points[4] = threeDPoint(5, 25, 65.5);
	points[5] = threeDPoint(100, 541, -12);

	SPKDArray kdArray = spKDArrayInit(points, 6);

	SPKDTreeNode treeRoot = spKDTreeBuild(kdArray, TREE_SPLIT_METHOD_INCREMENTAL);
	ASSERT(innerNodeState(treeRoot, 0, 3));

	SPKDTreeNode leftTree = spKDTreeNodeGetLeftChild(treeRoot);
	ASSERT(innerNodeState(leftTree, 1, 60));
	SPKDTreeNode leftTreeLeftChild = spKDTreeNodeGetLeftChild(leftTree);
	ASSERT(innerNodeState(leftTreeLeftChild, 2, -5.5));
	ASSERT(leafNodeState(spKDTreeNodeGetLeftChild(leftTreeLeftChild), points[0]));
	ASSERT(leafNodeState(spKDTreeNodeGetRightChild(leftTreeLeftChild), points[3]));
	ASSERT(leafNodeState(spKDTreeNodeGetRightChild(leftTree), points[1]));

	SPKDTreeNode rightTree = spKDTreeNodeGetRightChild(treeRoot);
	ASSERT(innerNodeState(rightTree, 1, 140.5));
	SPKDTreeNode rightTreeLeftChild = spKDTreeNodeGetLeftChild(rightTree);
	ASSERT(innerNodeState(rightTreeLeftChild, 2, 7.5));
	ASSERT(leafNodeState(spKDTreeNodeGetLeftChild(rightTreeLeftChild), points[2]));
	ASSERT(leafNodeState(spKDTreeNodeGetRightChild(rightTreeLeftChild), points[4]));
	ASSERT(leafNodeState(spKDTreeNodeGetRightChild(rightTree), points[5]));

	spKDArrayFreePointsArray(points, 6);
	spKDArrayDestroy(kdArray);
	spKDTreeDestroy(treeRoot);
	return true;
}

static bool kdTreeMaxSpreadProperBuildTest() {
	SPPoint *points = (SPPoint *) malloc(5 * sizeof(*points));
	points[0] = threeDPoint(1, 60, -5.5);
	points[1] = threeDPoint(123, 70, -4.5);
	points[2] = threeDPoint(2, 80, 4.5);
	points[3] = threeDPoint(9, 140.5, 7.5);
	points[4] = threeDPoint(3, 8, 133.5);

	SPKDArray kdArray = spKDArrayInit(points, 5);

	SPKDTreeNode treeRoot = spKDTreeBuild(kdArray, TREE_SPLIT_METHOD_MAX_SPREAD);
	ASSERT(innerNodeState(treeRoot, 2, 4.5));

	SPKDTreeNode leftTree = spKDTreeNodeGetLeftChild(treeRoot);
	ASSERT(innerNodeState(leftTree, 0, 2));
	SPKDTreeNode leftTreeLeftChild = spKDTreeNodeGetLeftChild(leftTree);
	ASSERT(innerNodeState(leftTreeLeftChild, 1, 60));
	ASSERT(leafNodeState(spKDTreeNodeGetRightChild(leftTree), points[1]));
	ASSERT(leafNodeState(spKDTreeNodeGetLeftChild(leftTreeLeftChild), points[0]));
	ASSERT(leafNodeState(spKDTreeNodeGetRightChild(leftTreeLeftChild), points[2]));

	SPKDTreeNode rightTree = spKDTreeNodeGetRightChild(treeRoot);
	ASSERT(innerNodeState(rightTree, 1, 8));
	ASSERT(leafNodeState(spKDTreeNodeGetLeftChild(rightTree), points[4]));
	ASSERT(leafNodeState(spKDTreeNodeGetRightChild(rightTree), points[3]));

	spKDArrayFreePointsArray(points, 5);
	spKDArrayDestroy(kdArray);
	spKDTreeDestroy(treeRoot);
	return true;
}

static bool innerNodeState(SPKDTreeNode treeNode, int expectedDimension, double expectedMedian) {
	ASSERT_NOT_NULL(treeNode);
	ASSERT_SAME(spKDTreeNodeGetDimension(treeNode), expectedDimension);
	ASSERT_SAME(spKDTreeNodeGetMedianValue(treeNode), expectedMedian);
	ASSERT_NULL(spKDTreeNodeGetData(treeNode));
	return true;
}

static bool leafNodeState(SPKDTreeNode treeNode, SPPoint expectedData) {
	ASSERT_NOT_NULL(treeNode);
	ASSERT_SAME(spKDTreeNodeGetDimension(treeNode), -1);
	ASSERT_SAME(spKDTreeNodeGetMedianValue(treeNode), INFINITY);
	ASSERT_NULL(spKDTreeNodeGetLeftChild(treeNode));
	ASSERT_NULL(spKDTreeNodeGetRightChild(treeNode));
	ASSERT(pointsEqualNotSame(*(spKDTreeNodeGetData(treeNode)), expectedData));
	return true;
}


int main() {
	RUN_TEST(kdTreeMaxSpreadProperBuildTest);
	RUN_TEST(kdTreeIncrementalProperBuildTest);
}
