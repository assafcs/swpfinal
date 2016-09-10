/*
 * SPKDArray.h
 *
 *  Created on: Jul 17, 2016
 *      Author: mataneilat
 */

#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

#include "SPPoint.h"

/**
 * Implementation of a k-dimensional array - a data-structure used to efficiently create a kd-tree - by providing the ability
 * to split the array with respect to a coordinate.
 *
 * The following functions are available:
 *
 * 		spKDArrayInit				- Initializes the kd-array with the given points.
 * 		spKDArrayCopy				- Copies the given kd-array.
 * 		spKDArraySplit				- Splits the array with respect to the given coordinate.
 * 		spKDArrayGetSpread			- Returns the spread of the values with respect to a given coordinate.
 * 		spKDArrayGetMedian			- Returns the median of the values with respect to a given coordinate.
 * 		spKDArrayMaxSpreadDimension - Returns the coordinate with the maximum value spread.
 * 		spKDArrayDestroy			- Deallocates the given kdArray.
 * 		spKDArrayGetSize			- Returns the number of points in the kdArray.
 * 		spKDArrayGetPointsDimension - Returns the kdArray points' dimension.
 * 		spKDArrayGetPointsArrayCopy - Returns a copy of the points array, with no specific order.
 * 		spKDArrayFreePointsArray	- Deallocates a given points array.
 *
 */

/** Type for defining the kd-array. */
typedef struct sp_kd_array_t *SPKDArray;

typedef struct sp_split_result_t *SPKDArraySplitResult;

/**
 * Initializes a new kd-array with the given array of SPPoints.
 *
 * @param arr The array of points that the kd-array will be consisted of.
 * @param size The size of the points array.
 *
 * @return
 *  NULL - If allocations failed, arr is NULL or size is non-positive.
 * 	A new kd-array in case of success.
 */
SPKDArray spKDArrayInit(SPPoint *arr, int size);

/**
 * Creates a copy of the given kd-array.
 *
 * @param kdArr The kd-array to copy.
 *
 * @return
 * 	NULL on allocation failure, or if the given kd-array is NULL
 * 	A copy of the given kd-array.
 */
SPKDArray spKDArrayCopy(SPKDArray kdArr);

/**
 * Deallocates the given kd-array.
 *
 * @param kdArray The kd-array to deallocate
 */
void spKDArrayDestroy(SPKDArray kdArray);

/**
 * Splits the kd-array with respect to the given coordinate.
 *
 * @param kdArr The kd-array to split
 * @param coor The coordinate to split by.
 *
 * @return
 * 	NULL in case of:
 * 		Allocation failure
 * 		Given kd-array is NULL
 * 		Coordinate is negative
 * 		Coordinate is larger than the points' dimension.
 *
 * 	In case of success, returns a SPKDArraySplitResult containing the two kd-arrays (n being the size of the split array):
 *
 * 	left - A kd-array containing the ceil([n/2]) points with the smaller values with respect to the given coordinate.
 * 	right - A kd-array containing the floor([n/2]) points with the bigger values with respect to the given coordinate.
 *
 * 	In case the kd-array is consisted of only one point, the left array will contain the point, the right will be NULL.
 * 	Notice that an empty kd-array is not valid (the init method requires a non-empty array).
 */
SPKDArraySplitResult spKDArraySplit(SPKDArray kdArr, int coor);


/*** Access Methods ***/

/**
 * Returns a copy of the points array.
 * The order of points is not guaranteed, so no assumptions should be made regarding it.
 *
 * @param kdArray The kdArray whose points array is requested
 *
 * @return
 * 	NULL if allocation failed, or the given array is NULL.
 * 	A copy of the kd-array's points array - in no specific order, in case of success
 */
SPPoint *spKDArrayGetPointsArrayCopy(SPKDArray kdArray);

/**
 * Deallocates a given points array.
 *
 * @param pointsArray The array of points to deallocate.
 * @param size The points array size
 *
 */
void spKDArrayFreePointsArray(SPPoint *pointsArray, int size);

/**
 * Returns the size of kd-array, meaning the number of points in it.
 *
 * @param kdArray The kd-array whose size is requested.
 *
 * @return
 *  -1 if the given kd-array is NULL.
 *  Otherwise returns the size of the given kd-array, meaning the number of points in it.
 */
int spKDArrayGetSize(SPKDArray kdArray);

/**
 * Returns the dimension of points in the given kd-array.
 *
 * @param kdArray The kd-array whose points' dimension is required.
 *
 * @return
 * 	-1 if the given kd-array is NULL
 * 	Otherwise returns the dimension of points in the given kd-array.
 */
int spKDArrayGetPointsDimension(SPKDArray kdArray);


/*** Aggregation Methods ***/

/**
 * Returns the spread of the points' values with respect to the given coordinate.
 *
 * @param kdArr The kd-array whose points' spread is required
 * @param coor The point's spread coordinate
 *
 * @return
 * 	-1 If the kd-array is NULL, or coordinate is negative or larger the array's points' dimension.
 * 	Otherwise returns the spread of the point's values with respect to the given coordinate.
 */
double spKDArrayGetSpread(SPKDArray kdArr, int coor);

/**
 * Returns the median of the points' values with respect to the given coordinate.
 *
 * @param kdArr The kd-array whose points' method is required
 * @param coor The point's median value coordinate
 *
 * @return
 * 	-1 If the kd-array is NULL, or coordinate is negative or larger the array's points' dimension.
 * 	Otherwise returns the median of the point's values with respect to the given coordinate.
 */
double spKDArrayGetMedian(SPKDArray kdArr, int coor);

/**
 * Returns the coordinate with the maximum spread.
 *
 * @param kdArr The kdArray whose max spread dimension is required.
 *
 * @return
 * 	-1 if the kd-array is NULL
 * 	Otherwise returns the coordinate with the maximum spread.
 */
int spKDArrayMaxSpreadDimension(SPKDArray kdArr);




/**
 * Implementation of the kdArray split result - a structure containing the left and right arrays.
 *
 * This data structure is create by the kdArrays split method - spKDArraySplit.
 *
 * The following functions are available:
 *
 * 		spKDArraySplitResultGetLeft			- Returns the left array of the split result.
 * 		spKDArraySplitResultGetRight		- Returns the right array of the split result.
 * 		spKDArraySplitResultDestroy			- Deallocates the split result
 *
 */

/**
 * Returns the left array of the split result.
 *
 * @param splitResult The split result to get it's left.
 *
 * @return
 * 	NULL if the given split result is NULL.
 * 	Otherwise returns the left array of the split result.
 */
SPKDArray spKDArraySplitResultGetLeft(SPKDArraySplitResult splitResult);

/**
 * Returns the right array of the split result.
 *
 * @param splitResult The split result to get it's right.
 *
 * @return
 * 	NULL if the given split result is NULL.
 * 	Otherwise returns the right array of the split result.
 */
SPKDArray spKDArraySplitResultGetRight(SPKDArraySplitResult splitResult);

/**
 * Deallocates the given split result
 *
 * @param splitResult The split result to deallocate.
 *
 */
void spKDArraySplitResultDestroy(SPKDArraySplitResult splitResult);

#endif /* SPKDARRAY_H_ */
