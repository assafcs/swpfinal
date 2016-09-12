/*
 * SPKDTree.h
 *
 *  Created on: Jul 19, 2016
 *      Author: mataneilat
 */

#ifndef SPKDTREE_H_
#define SPKDTREE_H_

#include "SPKDArray.h"
#include "SPConfig.h"

/**
 * Implementation of a kd-tree, which is a space partitioning data structure for organizing points in a k-dimensional space.
 * Each inner node in the tree contains information about the split dimension and median value, point data is in the leaf.
 *
 * For example, in a 3 dimensional space, each point has (x,y,z) values:
 * 	x being the 0 dimension
 * 	y being the 1 dimension
 * 	z being the 2 dimension
 *
 * Given an inner tree node, with median value m, and split dimension d (in {0,1,2}):
 * 		Points residing in leaves on the left sub-tree will have <=m values in the d coordinate.
 * 		Points residing in leaves on the right sub-tree will have >m values in the d coordinate.
 *
 * The following functions are available:
 *
 * 		spKDTreeBuild				- Builds the kd-tree from the given kd-array using the given split method.
 * 		spKDTreeDestroy				- Deallocates the given kd-tree
 * 		spKDTreeNodeIsLeaf 			- Returns whether the given tree node is considered a leaf.
 * 		spKDTreeNodeGetDimension	- Returns the tree node's split dimension
 * 		spKDTreeNodeGetMedianValue	- Returns the tree node's median value with respect to the split dimension
 * 		spKDTreeNodeGetLeftChild	- Returns the tree node's left child
 * 		spKDTreeNodeGetRightChild	- Returns the tree node's right child
 * 		spKDTreeNodeGetData			- Returns the tree node's data - resides only in the leaves.
 */

/** Type for defining the kd-tree. */
typedef struct sp_kd_tree_node_t *SPKDTreeNode;

/**
 * Builds a kd-tree using the given kd-array and the desired split method.
 *
 * @param kdArray The kd-array used to build the tree with. Contains the points data organized efficiently to split on a specific axis.
 * @param splitMethod The desired method to split the tree according to.
 * 		TREE_SPLIT_METHOD_RANDOM - Split the space with respect to a random dimension.
 * 		TREE_SPLIT_METHOD_MAX_SPREAD - Split the space with respect to the dimension with the maximum spread (maximum point's coordinate diff).
 * 		TREE_SPLIT_METHOD_INCREMENTAL - Split the space with respect to an incrementing dimension in each level of the tree.
 *
 * @return
 * 	NULL in case the given kd-array is NULL, or an allocation failure occurred.
 * 	Otherwise, return the root of the newly create tree.
 */
SPKDTreeNode spKDTreeBuild(SPKDArray kdArray, SP_TREE_SPLIT_METHOD splitMethod);

/**
 * Deallocates the kd-tree descending from the given tree root node.
 * Frees all tree node data and point data on leaves.
 *
 * @param treeNode The root node represnting the tree to deallocate.
 *
 */
void spKDTreeDestroy(SPKDTreeNode treeNode);

/*** Access Methods ***/

/**
 * Returns whether the given tree node is a leaf or not.
 *
 * @param treeNode The queried tree node.
 *
 * @return
 * 	false if the given tree node is NULL.
 * 	Otherwise, return whether the given tree node is a leaf.
 */
bool spKDTreeNodeIsLeaf(SPKDTreeNode treeNode);

/**
 * Returns the dimension which the descending nodes were split by.
 *
 * @param treeNode The queried tree node.
 *
 * @return
 * 	-1 if the given tree node is NULL (or a leaf).
 * 	Otherwise, return the dimension that was used to split the space for descending nodes.
 */
int spKDTreeNodeGetDimension(SPKDTreeNode treeNode);

/**
 * Returns the median value of the descending points with respect to the split dimension.
 *
 * Points residing in leaves on the left sub-tree will have smaller or equal values in the split-axis coordinate.
 * Points residing in leaves on the right sub-tree will have bigger values in the split-axis coordinate.
 *
 * @param treeNode The queried tree node
 *
 * @return
 * 	INFINITY if the given tree node is NULL (or a leaf).
 * 	Otherwise, return the median value of the descending points with respect to the split dimension.
 */
double spKDTreeNodeGetMedianValue(SPKDTreeNode treeNode);

/**
 * Returns the left child of the given tree node.
 *
 * @param treeNode The node to return its left child.
 *
 * @return
 * 	NULL if the given tree node is NULL, or the left child does not exist.
 * 	Otherwise, return the tree node's left child.
 */
SPKDTreeNode spKDTreeNodeGetLeftChild(SPKDTreeNode treeNode);

/**
 * Returns the right child of the given tree node.
 *
 * @param treeNode The node to return its right child.
 *
 * @return
 * 	NULL if the given tree node is NULL, or the right child does not exist.
 * 	Otherwise, return the tree node's right child.
 */
SPKDTreeNode spKDTreeNodeGetRightChild(SPKDTreeNode treeNode);

/**
 * Returns a copy of the tree node's data.
 * Data only resides in leaves (as a single point), thus this returns NULL for a non-leaf node.
 *
 * @param treeNode The queries tree node.
 *
 * @return
 * 	NULL if the given tree node is NULL, or not a leaf.
 * 	Otherwise, returns the tree node's point data.
 */
SPPoint *spKDTreeNodeGetData(SPKDTreeNode treeNode);

#endif /* SPKDTREE_H_ */
