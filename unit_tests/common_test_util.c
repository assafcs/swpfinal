/*
 * common_test_util.c
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */


#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include "common_test_util.h"
#include "unit_test_util.h"

bool pointsArrayEqualNotSame(SPPoint* aPointsArray, SPPoint *bPointsArray, int size) {
	int i;
	ASSERT_NOT_SAME(aPointsArray, bPointsArray);
	for (i = 0; i < size; i++) {
		ASSERT(pointsEqualNotSame(aPointsArray[i], bPointsArray[i]));
	}
	return true;
}

bool pointsEqualNotSame(SPPoint aPoint, SPPoint bPoint) {
	int i;
	int pointDimention = spPointGetDimension(aPoint);
	ASSERT_NOT_SAME(aPoint, bPoint);
	ASSERT_SAME(pointDimention, spPointGetDimension(bPoint));
	ASSERT_SAME(spPointGetIndex(aPoint), spPointGetIndex(bPoint));
	for (i = 0; i < pointDimention; i++) {
		ASSERT_SAME(spPointGetAxisCoor(aPoint, i), spPointGetAxisCoor(bPoint, i));
	}
	return true;
}

SPPoint twoDPoint(double x, double y) {
	double *pointData = (double *) malloc(2 * sizeof(double));
	pointData[0] = x;
	pointData[1] = y;
	SPPoint point = spPointCreate(pointData, 2, 0);
	free(pointData);
	return point;
}

SPPoint threeDPoint(double x, double y, double z) {
	return indexedThreeDPoint(0, x, y, z);
}

SPPoint indexedThreeDPoint(int index, double x, double y, double z) {
	double *pointData = (double *) malloc(3 * sizeof(double));
	pointData[0] = x;
	pointData[1] = y;
	pointData[2] = z;
	SPPoint point = spPointCreate(pointData, 3, index);
	free(pointData);
	return point;
}

SPPoint nDPoint(int index, int n, ...) {
	va_list ap;
	int i;
	va_start(ap, n);
	double *pointData = (double *) malloc(n * sizeof(double));
	for (i = 0; i < n; i++) {
		pointData[i] = va_arg(ap, double);
	}
	va_end(ap);
	SPPoint point = spPointCreate(pointData, n, index);
	free(pointData);
	return point;
}

