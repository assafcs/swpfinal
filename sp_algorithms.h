/*
 * algorithms.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#ifndef SP_ALGORITHMS_H_
#define SP_ALGORITHMS_H_

#include <stdlib.h>
#include "SPBPriorityQueue.h"
#include "SPKDTree.h"
#include "SPPoint.h"

/**
 * Algorithms implementations.
 *
 * The following functions are available:
 * 		spKNearestNeighbours 			- Implementation of a nearest neighbor algorithm.
 *
 */

/**
 * Implementation of a nearest neighbor algorithm.
 * Fills the priority queue with the "nearest" features in the tree, based on squared distance.
 *
 * @param tree The kd-tree representing the points in the space.
 * @param queue The priority queue to hold the nearest neighbors (meaning the closest features in the kd-tree).
 * @param point The feature to search for its nearest neighbors.
 */
void spKNearestNeighbours(SPKDTreeNode tree, SPBPQueue queue, SPPoint point);

#endif /* SP_ALGORITHMS_H_ */
