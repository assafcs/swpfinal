/*
 * sp_kd_array_test.c
 *
 *  Created on: Jul 18, 2016
 *      Author: mataneilat
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
#include "unit_test_util.h"
#include "common_test_util.h"
#include "../SPKDArray.h"

static bool kdArrayDimensionInfo(SPKDArray arr, int coor, double expectedSpread, double expectedMedian);
static bool kdArrayState(SPKDArray kdArray, SPPoint *expectedPointsArray, int expectedSize);

static bool kdArrayInitTest() {
	SPPoint *points = (SPPoint *) malloc(3 * sizeof(*points));
	points[0] = twoDPoint(4, 7);
	points[1] = twoDPoint(3, 8);
	points[2] = twoDPoint(3.5, 7.5);

	SPKDArray kdArray = spKDArrayInit(points, 3);
	ASSERT(kdArrayState(kdArray, points, 3));

	spKDArrayFreePointsArray(points, 3);
	spKDArrayDestroy(kdArray);
	return true;
}

static bool kdArraySplitTest() {
	SPPoint *points = (SPPoint *) malloc(5 * sizeof(*points));
	points[0] = twoDPoint(1, 2);
	points[1] = twoDPoint(123, 70);
	points[2] = twoDPoint(2, 7);
	points[3] = twoDPoint(9, 11);
	points[4] = twoDPoint(3, 4);

	SPKDArray kdArray = spKDArrayInit(points, 5);
	ASSERT(kdArrayState(kdArray, points, 5));

	SPKDArraySplitResult splitResult = spKDArraySplit(kdArray, 0);

	SPPoint *leftPoints = (SPPoint *) malloc(3 * sizeof(*leftPoints));
	leftPoints[0] = points[0];
	leftPoints[1] = points[2];
	leftPoints[2] = points[4];

	SPPoint *rightPoints = (SPPoint *) malloc(2 * sizeof(*rightPoints));
	rightPoints[0] = points[3];
	rightPoints[1] = points[1];

	ASSERT(kdArrayState(spKDArraySplitResultGetLeft(splitResult), leftPoints, 3));
	ASSERT(kdArrayState(spKDArraySplitResultGetRight(splitResult), rightPoints, 2));

	spKDArrayFreePointsArray(points, 5);
	free(leftPoints);
	free(rightPoints);

	spKDArrayDestroy(kdArray);
	spKDArraySplitResultDestroy(splitResult);
	return true;
}

static bool kdArrayDimensionInfoTest() {
	SPPoint *points = (SPPoint *) malloc(5 * sizeof(*points));
	points[0] = threeDPoint(1, 2, -5.5);
	points[1] = threeDPoint(123, 70, -4.5);
	points[2] = threeDPoint(2, 7, 4.5);
	points[3] = threeDPoint(9, 11, 7.5);
	points[4] = threeDPoint(3, 4, 133.5);

	SPKDArray kdArray = spKDArrayInit(points, 5);
	ASSERT(kdArrayState(kdArray, points, 5));

	ASSERT(kdArrayDimensionInfo(kdArray, 0, 122, 3));
	ASSERT(kdArrayDimensionInfo(kdArray, 1, 68, 7));
	ASSERT(kdArrayDimensionInfo(kdArray, 2, 139, 4.5));

	ASSERT_SAME(spKDArrayMaxSpreadDimension(kdArray), 2);

	spKDArrayFreePointsArray(points, 5);
	spKDArrayDestroy(kdArray);
	return true;
}

static bool kdArrayInitEmptyArrayTest() {
	SPPoint *points = (SPPoint *) malloc(0);
	SPKDArray kdArray = spKDArrayInit(points, 0);

	ASSERT_NULL(kdArray);
	spKDArrayFreePointsArray(points, 0);
	spKDArrayDestroy(kdArray);

	return true;
}

static bool kdArraySplitOnePointArrayTest() {
	SPPoint *points = (SPPoint *) malloc(sizeof(*points));
	points[0] = threeDPoint(0, 0, 0);
	SPKDArray kdArray = spKDArrayInit(points, 1);
	ASSERT(kdArrayState(kdArray, points, 1));

	SPKDArraySplitResult splitResult = spKDArraySplit(kdArray, 1);
	ASSERT(kdArrayState(spKDArraySplitResultGetLeft(splitResult), points, 1));
	ASSERT_NULL(spKDArraySplitResultGetRight(splitResult));

	spKDArrayFreePointsArray(points, 1);
	spKDArrayDestroy(kdArray);
	spKDArraySplitResultDestroy(splitResult);

	return true;

}

/*** Helper assertion methods ***/

static bool kdArrayDimensionInfo(SPKDArray arr, int coor, double expectedSpread, double expectedMedian) {
	ASSERT_NOT_NULL(arr);
	ASSERT_SAME(spKDArrayGetSpread(arr, coor), expectedSpread);
	ASSERT_SAME(spKDArrayGetMedian(arr, coor), expectedMedian);
	return true;
}

static bool kdArrayState(SPKDArray kdArray, SPPoint *expectedPointsArray, int expectedSize) {
	ASSERT_NOT_NULL(kdArray);
	ASSERT_SAME(spKDArrayGetSize(kdArray), expectedSize);

	SPPoint *pointsArray = spKDArrayGetPointsArrayCopy(kdArray);
	ASSERT(pointsArrayEqualNotSame(pointsArray, expectedPointsArray, expectedSize));
	spKDArrayFreePointsArray(pointsArray, expectedSize);
	return true;
}

int main() {
	RUN_TEST(kdArrayInitTest);
	RUN_TEST(kdArraySplitTest);
	RUN_TEST(kdArrayInitEmptyArrayTest);
	RUN_TEST(kdArraySplitOnePointArrayTest);
	RUN_TEST(kdArrayDimensionInfoTest);
}
