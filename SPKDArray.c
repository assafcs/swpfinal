/*
 * SPKDArray.c
 *
 *  Created on: Jul 17, 2016
 *      Author: mataneilat
 */

#include "SPKDArray.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/*** Inner implementation type declarations ***/

/** Structure containing both index and value - used for sorting. */
typedef struct index_to_value {
	int index;
	double value;
} IndexToValue;

/** Structure containing index and array identifier - used for array split. */
typedef struct split_index_mapping {
	int arrIdentifier; // 0 is left, 1 is right
	int index;
} SplitIndexMapping;


/*** Type declarations ***/

/** Structure containing the kdArray data. */
struct sp_kd_array_t {
	int **indicesMatrix;
	SPPoint* pointsArray;
	int size;
};

/** Structure containing the split result data. */
struct sp_split_result_t {
	SPKDArray left;
	SPKDArray right;
};


/*** Private Methods ***/

/**
 * Compare method between two IndexToValue elements.
 *
 * @param aIndexToValuePtr Pointer to the first IndexToValue element.
 * @param bIndexToValuePtr Pointer to the second IndexToValue element.
 *
 * @return
 * 	0 	- If the elements are considered equal
 * 	-1 	- If the first element is smaller than the second.
 * 	1 	- If the second element is smaller than the first
 */
int comparePointsCoodinates(const void *aIndexToValuePtr, const void *bIndexToValuePtr) {
	IndexToValue *aIndexToValue = (IndexToValue *) aIndexToValuePtr;
	IndexToValue *bIndexToValue = (IndexToValue *) bIndexToValuePtr;
	if (aIndexToValue->value == bIndexToValue->value) {
		return 0;
	}
	return (aIndexToValue->value < bIndexToValue->value) ? -1 : 1;
}

/**
 * Returns the points' indices in the array sorted by the points' values on the given axis.
 *
 * @param pointsArray The array of points
 * @param size The size of the points array
 * @param axis The axis to sort by.
 *
 * @retrun
 * 	Array of the points indices sorted by the points' values on the given axis.
 */
int *sortedIndices(SPPoint *pointsArray, int size, int axis) {
	int i;
	IndexToValue indexToValue;
	IndexToValue *indicesToValues = (IndexToValue *) malloc(sizeof(IndexToValue) * size);
	int *indicesArray = (int *) malloc (sizeof(int) * size);
	if (indicesToValues == NULL || indicesArray == NULL) {
		return NULL;
	}
	for (i = 0; i < size; i++) {
		indexToValue = (IndexToValue){i, spPointGetAxisCoor(pointsArray[i], axis)};
		indicesToValues[i] = indexToValue;
	}
	qsort(indicesToValues, size, sizeof(IndexToValue), comparePointsCoodinates);

	for (i = 0; i < size; i++) {
		indicesArray[i] = indicesToValues[i].index;
	}
	free(indicesToValues);
	return indicesArray;
}

/**
 * Copies an array of points.
 *
 * @param pointsArray The array of points to copy.
 * @param size The size of the array.
 *
 * @return
 * 	A new copy of the points array.
 */
SPPoint *copyPointsArray(SPPoint *pointsArray, int size) {
	int i;
	SPPoint pointCopy;
	SPPoint *arrCopy = (SPPoint *) malloc(sizeof(*pointsArray) * size);
	if (arrCopy == NULL) {
		return NULL;
	}
	for (i = 0; i < size; i++) {
		pointCopy = spPointCopy(pointsArray[i]);
		if (pointCopy == NULL) {
			spKDArrayFreePointsArray(arrCopy, size);
			return NULL;
		}
		arrCopy[i] = pointCopy;
	}
	return arrCopy;
}

/**
 * Creates an indices matrix.
 * Each row of the matrix contains the indices of the points, sorted by the points' values with respect to the proper coordinate.
 *
 * @param pointsArray The points array for which to create the indices matrix.
 * @param size The size of the points array (The matrix column count)
 * @param pointsDimension The dimension of the points (The matrix row count)
 *
 * @return
 * 	The indices matrix - each row contains the indices of the points in the array,
 * 	sorted by the points' values with respect to the proper coordinate.
 */
int **createIndicesMatrix(SPPoint *pointsArray, int size, int pointsDimension) {
	int i, *axisIndices;
	int **indicesMatrix = (int **) malloc(sizeof(*indicesMatrix) * pointsDimension);
	if (indicesMatrix == NULL) {
		return NULL;
	}
	for (i = 0; i < pointsDimension; i++) {
		axisIndices = sortedIndices(pointsArray, size, i);
		if (axisIndices == NULL) {
			spKDArrayFreeIndicesMatrix(indicesMatrix, i);
			return NULL;
		}
		indicesMatrix[i] = axisIndices;
	}
	return indicesMatrix;
}

/**
 * Creates a copy of the given indices matrix.
 *
 * @param indicesMatrix The indices matrix to copy.
 * @param size The number of columns in the matrix (the size of each row).
 * @param pointsDimension The number of rows in the matrix.
 *
 * @return
 * 	A copy of the given indices matrix.
 */
int **copyIndicesMatrix(int** indicesMatrix, int size, int pointsDimension) {
	int i, *row;
	if (indicesMatrix == NULL || size < 0 || pointsDimension < 0) {
		return NULL;
	}
	int **indicesMatrixCopy = (int **) malloc(sizeof(*indicesMatrix) * pointsDimension);
	if (indicesMatrixCopy == NULL) {
		return NULL;
	}
	for (i = 0; i < pointsDimension; i++) {
		indicesMatrixCopy[i] = (int *) malloc(sizeof(*row) * size);
		if (indicesMatrixCopy[i] == NULL) {
			spKDArrayFreeIndicesMatrix(indicesMatrix, i);
			return NULL;
		}
		memcpy(indicesMatrixCopy[i], indicesMatrix[i], size * sizeof(int));
	}
	return indicesMatrixCopy;
}

/**
 * Helper method to free the given variables.
 *
 * @param splitResult The SPKDArraySplitResult to destroy.
 * @param indexMapping The SplitIndexMapping array to free.
 */
void freeSplitVariables(SPKDArraySplitResult splitResult, SplitIndexMapping *indexMapping) {
	free(indexMapping);
	spKDArraySplitResultDestroy(splitResult);
}

/**
 * Helper method to fill the split result for a single point array.
 *
 * @param kdArr The single point kd-array to split.
 * @param splitResult The split result to fill.
 */
void splitSingleItemArray(SPKDArray kdArr, SPKDArraySplitResult splitResult) {
	if (kdArr == NULL || splitResult == NULL) {
		return;
	}
	splitResult->right = NULL;
	splitResult->left = spKDArrayCopy(kdArr);
}

/*** Public Methods ***/

SPKDArray spKDArrayInit(SPPoint *arr, int size) {
	if (arr == NULL || size <= 0) {
		return NULL;
	}
	int pointsDimension = spPointGetDimension(arr[0]);
	SPKDArray kdArray = (SPKDArray ) malloc(sizeof(*kdArray));
	if (kdArray == NULL) {
		return NULL;
	}
	int **indicesMatrix = createIndicesMatrix(arr, size, pointsDimension);
	if (indicesMatrix == NULL) {
		spKDArrayDestroy(kdArray);
		return NULL;
	}
	SPPoint *arrCopy = copyPointsArray(arr, size);
	if (arrCopy == NULL) {
		spKDArrayDestroy(kdArray);
		return NULL;
	}

	kdArray->pointsArray = arrCopy;
	kdArray->indicesMatrix = indicesMatrix;
	kdArray->size = size;

	return kdArray;
}

SPKDArray spKDArrayCopy(SPKDArray kdArr) {
	if (kdArr == NULL) {
		return NULL;
	}
	SPKDArray kdArrCopy = (SPKDArray) malloc(sizeof(*kdArrCopy));
	if (kdArrCopy == NULL) {
		return NULL;
	}
	kdArrCopy->size = spKDArrayGetSize(kdArr);
	kdArrCopy->pointsArray = spKDArrayGetPointsArrayCopy(kdArr);
	kdArrCopy->indicesMatrix = spKDArrayGetIndicesMatrixCopy(kdArr);
	return kdArrCopy;
}

void spKDArrayDestroy(SPKDArray kdArray) {
	if (kdArray == NULL) {
		return;
	}
	// Order here is important, since the dimension relies on the points array..
	spKDArrayFreeIndicesMatrix(kdArray->indicesMatrix, spKDArrayGetPointsDimension(kdArray));
	spKDArrayFreePointsArray(kdArray->pointsArray, kdArray->size);
	free(kdArray);
}

SPKDArraySplitResult spKDArraySplit(SPKDArray kdArr, int coor) {
	if (kdArr == NULL || coor < 0 || coor > spKDArrayGetPointsDimension(kdArr)) {
		return NULL;
	}

	// Variables declarations
	int i, j, currentIndex, leftPointIndex, rightPointIndex, currentArrIdentifier;
	SPPoint pointCopy = NULL;
	SPKDArray currentArr = NULL;
	SplitIndexMapping currentIndexMapping;
	int *leftIndices, *rightIndices, *currentPointIndex;
	int pointsDimension = spKDArrayGetPointsDimension(kdArr);
	int size = kdArr->size;
	int **indicesMatrix = kdArr->indicesMatrix;
	int *sortedIndices = indicesMatrix[coor];


	// Create the split result
	SPKDArraySplitResult splitResult = (SPKDArraySplitResult) malloc(sizeof(*splitResult));
	if (splitResult == NULL) {
		return NULL;
	}

	// In case there is only one item in the array, return it as left
	if (spKDArrayGetSize(kdArr) == 1) {
		splitSingleItemArray(kdArr, splitResult);
		return splitResult;
	}

	// Create the SplitIndexMapping used for the split logic
	SplitIndexMapping *indexMapping = (SplitIndexMapping *) malloc(size * sizeof(SplitIndexMapping));
	if (indexMapping == NULL) {
		spKDArraySplitResultDestroy(splitResult);
		return NULL;
	}

	// Allocate the left and right kd-arrays
	splitResult->left = (SPKDArray) malloc(sizeof(*(splitResult->left)));
	splitResult->right = (SPKDArray) malloc(sizeof(*(splitResult->right)));

	if (splitResult->left == NULL || splitResult->right == NULL) {
		freeSplitVariables(splitResult, indexMapping);
		return NULL;
	}

	// Calculate the number of items for left and right arrays.
	splitResult->left->size = (int)ceil((double)kdArr->size / 2.0);
	splitResult->right->size = (int)floor((double)kdArr->size / 2.0);

	// Allocate the left and right point arrays.
	splitResult->left->pointsArray = (SPPoint *) malloc(splitResult->left->size * sizeof(SPPoint));
	splitResult->right->pointsArray = (SPPoint *) malloc(splitResult->right->size * sizeof(SPPoint));

	// Allocate the left and right indices matrices
	splitResult->left->indicesMatrix = (int **) malloc(pointsDimension * sizeof(int *));
	splitResult->right->indicesMatrix = (int **) malloc(pointsDimension * sizeof(int *));

	// Return NULL in case allocation failed
	if (splitResult->left->pointsArray == NULL || splitResult->right->pointsArray == NULL
			|| splitResult->left->indicesMatrix == NULL || splitResult->right->indicesMatrix == NULL) {
		freeSplitVariables(splitResult, indexMapping);
		return NULL;
	}

	// Create the index mapping and fill the points in the split result arrays.
	leftPointIndex = 0;
	rightPointIndex = 0;
	for (i = 0; i < size; i++) {
		currentIndex = sortedIndices[i];
		if (i < splitResult->left->size) {
			currentArr = splitResult->left;
			currentPointIndex = &leftPointIndex;
			currentArrIdentifier = 0;
		} else {
			currentArr = splitResult->right;
			currentPointIndex = &rightPointIndex;
			currentArrIdentifier = 1;
		}
		pointCopy = spPointCopy(kdArr->pointsArray[currentIndex]);
		currentArr->pointsArray[*currentPointIndex] = pointCopy;
		indexMapping[currentIndex] = (SplitIndexMapping) {currentArrIdentifier, *currentPointIndex};
		(*currentPointIndex)++;
	}

	// Use the index mapping to create the indices matrices of the split result arrays.
	for (i = 0; i < pointsDimension; i++) {
		leftIndices = (int *) malloc(splitResult->left->size * sizeof(int));
		rightIndices = (int *) malloc(splitResult->right->size * sizeof(int));

		if (leftIndices == NULL || rightIndices == NULL) {
			freeSplitVariables(splitResult, indexMapping);
			return NULL;
		}
		splitResult->left->indicesMatrix[i] = leftIndices;
		splitResult->right->indicesMatrix[i] = rightIndices;

		leftPointIndex = 0;
		rightPointIndex = 0;
		for (j = 0; j < size; j++) {
			currentIndex = kdArr->indicesMatrix[i][j];
			currentIndexMapping = indexMapping[currentIndex];
			if (currentIndexMapping.arrIdentifier == 0) {
				currentArr = splitResult->left;
				currentPointIndex = &leftPointIndex;
			} else {
				currentArr = splitResult->right;
				currentPointIndex = &rightPointIndex;
			}
			currentArr->indicesMatrix[i][*currentPointIndex] = currentIndexMapping.index;
			(*currentPointIndex)++;
		}
	}

	free(indexMapping);

	return splitResult;
}

SPPoint *spKDArrayGetPointsArrayCopy(SPKDArray kdArray) {
	if (kdArray == NULL) return NULL;
	return copyPointsArray(kdArray->pointsArray, spKDArrayGetSize(kdArray));
}

void spKDArrayFreePointsArray(SPPoint *pointsArray, int size) {
	int i;
	if (pointsArray == NULL) {
		return;
	}
	for (i = 0; i < size; i++) {
		spPointDestroy(pointsArray[i]);
	}
	free(pointsArray);
}

int **spKDArrayGetIndicesMatrixCopy(SPKDArray kdArray) {
	if (kdArray == NULL) return NULL;
	return copyIndicesMatrix(kdArray->indicesMatrix, spKDArrayGetSize(kdArray), spKDArrayGetPointsDimension(kdArray));
}

void spKDArrayFreeIndicesMatrix(int **indicesMatrix, int rows) {
	int i, *row;
	if (indicesMatrix == NULL) {
		return;
	}
	for (i = 0; i < rows; i++) {
		row = indicesMatrix[i];
		if (row != NULL) {
			free(row);
		}
	}
	free(indicesMatrix);
}


int spKDArrayGetSize(SPKDArray kdArray) {
	if (kdArray == NULL) return -1;
	return kdArray->size;
}

int spKDArrayGetPointsDimension(SPKDArray kdArray) {
	if (kdArray == NULL || kdArray->size == 0) return -1;
	return spPointGetDimension(kdArray->pointsArray[0]);
}

double spKDArrayGetSpread(SPKDArray kdArr, int coor) {
	int pointsDim, arrSize;
	int **indicesMatrix;
	int *sortedIndices = NULL;
	SPPoint minimalPoint, maximalPoint;
	if (kdArr == NULL) {
		return -1;
	}
	pointsDim = spKDArrayGetPointsDimension(kdArr);
	arrSize = kdArr->size;
	indicesMatrix = kdArr->indicesMatrix;
	if (arrSize <= 0) {
		// Empty array..
		return -1;
	}
	if (coor < 0 || coor > pointsDim) {
		// Invalid argument
		return -1;
	}
	sortedIndices = indicesMatrix[coor];
	minimalPoint = kdArr->pointsArray[sortedIndices[0]];
	maximalPoint = kdArr->pointsArray[sortedIndices[arrSize - 1]];
	return spPointGetAxisCoor(maximalPoint, coor) - spPointGetAxisCoor(minimalPoint, coor);
}

double spKDArrayGetMedian(SPKDArray kdArr, int coor) {
	int pointsDim, arrSize;
	int **indicesMatrix;
	int *sortedIndices = NULL;
	SPPoint medianPoint;
	if (kdArr == NULL) {
		return -1;
	}
	pointsDim = spKDArrayGetPointsDimension(kdArr);
	arrSize = kdArr->size;
	indicesMatrix = kdArr->indicesMatrix;
	if (arrSize <= 0) {
		// Empty array..
		return -1;
	}
	if (coor < 0 || coor > pointsDim) {
		// Invalid argument
		return -1;
	}
	sortedIndices = indicesMatrix[coor];
	medianPoint = kdArr->pointsArray[sortedIndices[(arrSize - 1) / 2]];
	return spPointGetAxisCoor(medianPoint, coor);
}

int spKDArrayMaxSpreadDimension(SPKDArray kdArr) {
	int dim, currentDimension, maxSpreadDimension = -1;
	double currentSpread, maxSpread = -1;
	if (kdArr == NULL) {
		return -1;
	}
	dim = spKDArrayGetPointsDimension(kdArr);
	if (dim <= 0) {
		return -1;
	}
	for (currentDimension = 0; currentDimension < dim; currentDimension++) {
		currentSpread = spKDArrayGetSpread(kdArr, currentDimension);
		if (currentSpread > maxSpread) {
			maxSpread = currentSpread;
			maxSpreadDimension = currentDimension;
		}
	}
	return maxSpreadDimension;
}

/*** Split Result Methods ***/

SPKDArray spKDArraySplitResultGetLeft(SPKDArraySplitResult splitResult) {
	if (splitResult == NULL) {
		return NULL;
	}
	return splitResult->left;
}

SPKDArray spKDArraySplitResultGetRight(SPKDArraySplitResult splitResult) {
	if (splitResult == NULL) {
		return NULL;
	}
	return splitResult->right;
}

void spKDArraySplitResultDestroy(SPKDArraySplitResult splitResult) {
	if (splitResult == NULL) {
		return;
	}
	spKDArrayDestroy(splitResult->left);
	spKDArrayDestroy(splitResult->right);
	free(splitResult);
}

