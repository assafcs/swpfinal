/*
 * sp_features_file_api.c
 *
 *  Created on: Sep 11, 2016
 *      Author: mataneilat
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sp_features_file_api.h"
#include "SPKDArray.h"
#include "sp_util.h"

static const int MAX_FEATURE_COORDINATE_STRING_LEN = 20;

static const int MAX_NUM_OF_FEATURES_STRING_LEN = 10;

static const char FEATURE_COORDINATES_DELIM = ' ';

int loadNumberOfFeatures(FILE *featuresFile, SP_FEATURES_FILE_API_MSG *msg) {
	int numberOfFeatures;
	char *numberOfFeaturesAsString = (char *) malloc((MAX_NUM_OF_FEATURES_STRING_LEN + 1) * sizeof(char));

	if (numberOfFeaturesAsString == NULL) {
		*msg = SP_FEATURES_FILE_API_ALLOC_FAIL;
		return -1;
	}
	if (fgets(numberOfFeaturesAsString, MAX_NUM_OF_FEATURES_STRING_LEN, featuresFile) == NULL) {
		free(numberOfFeaturesAsString);
		*msg = SP_FEATURES_FILE_API_LOAD_ERROR;
		return -1;
	}
	numberOfFeatures = atoi(numberOfFeaturesAsString);

	free(numberOfFeaturesAsString);
	*msg = (numberOfFeatures == 0) ? SP_FEATURES_FILE_API_LOAD_ERROR : SP_FEATURES_FILE_API_SUCCESS;
	return numberOfFeatures;
}

SPPoint loadFeature(FILE *featuresFile, int expectedDimension, int index, SP_FEATURES_FILE_API_MSG *msg) {
	int numberOfCoordinates;
	char **splitResult;
	double *data;
	SPPoint feature;
	int maxFeatureStringLen = (MAX_FEATURE_COORDINATE_STRING_LEN + 1) * expectedDimension;
	char *featureCoordinatesString = (char *) malloc((maxFeatureStringLen + 1) * sizeof(char));

	if (featureCoordinatesString == NULL) {
		*msg = SP_FEATURES_FILE_API_ALLOC_FAIL;
		return NULL;
	}
	if (fgets(featureCoordinatesString, maxFeatureStringLen, featuresFile) == NULL) {
		free(featureCoordinatesString);
		*msg = SP_FEATURES_FILE_API_LOAD_ERROR;
		return NULL;
	}


	splitResult = spUtilStrSplit(featureCoordinatesString, FEATURE_COORDINATES_DELIM, &numberOfCoordinates);
	// The feature coordinates string is no longer needed..
	free(featureCoordinatesString);

	if (splitResult == NULL || numberOfCoordinates != expectedDimension) {
		spUtilFreeStringsArray(splitResult, numberOfCoordinates);
		*msg = SP_FEATURES_FILE_API_LOAD_ERROR;
		return NULL;
	}

	data = (double *) malloc(numberOfCoordinates * sizeof(double));
	for (int i = 0; i < numberOfCoordinates; i++) {
		double featureCoordinate = atof(splitResult[i]);
		if (featureCoordinate == 0 && strcmp(splitResult[i], "0") != 0) {
			free(data);
			spUtilFreeStringsArray(splitResult, numberOfCoordinates);
			*msg = SP_FEATURES_FILE_API_LOAD_ERROR;
			return NULL;
		}
		data[i] = featureCoordinate;
	}
	feature = spPointCreate(data, numberOfCoordinates, index);
	free(data);
	spUtilFreeStringsArray(splitResult, numberOfCoordinates);
	*msg = SP_FEATURES_FILE_API_SUCCESS;
	return feature;
}

SPPoint *loadFeatures(const char *filePath, int index, int expectedFeatureDimension, int *numOfFeaturesLoaded,
		SP_FEATURES_FILE_API_MSG *msg) {
	FILE *featuresFile = fopen(filePath, "r");
	int i, j;
	if (featuresFile == NULL) {
		*msg = SP_FEATURES_FILE_API_FEATURE_FILE_MISSING;
		return NULL;
	}
	int numberOfFeatures = loadNumberOfFeatures(featuresFile, msg);
	if (*msg != SP_FEATURES_FILE_API_SUCCESS) {
		fclose(featuresFile);
		return NULL;
	}

	SPPoint *features = (SPPoint *) malloc(numberOfFeatures * sizeof(SPPoint));
	if (features == NULL) {
		fclose(featuresFile);
		*msg = SP_FEATURES_FILE_API_ALLOC_FAIL;
		return NULL;
	}
	for (i = 0; i < numberOfFeatures; i++) {
		SPPoint feature = loadFeature(featuresFile, expectedFeatureDimension, index, msg);
		if (*msg != SP_FEATURES_FILE_API_SUCCESS) {
			for (j = 0; j < i; j++) {
				spPointDestroy(features[j]);
			}
			free(features);
			fclose(featuresFile);
			return NULL;
		}
		features[i] = feature;
	}

	fclose(featuresFile);
	*msg = SP_FEATURES_FILE_API_SUCCESS;
	*numOfFeaturesLoaded = numberOfFeatures;
	return features;

}

SP_FEATURES_FILE_API_MSG writeFeature(FILE* featureFile, SPPoint feature) {
	if (featureFile == NULL) {
		return SP_FEATURES_FILE_API_INVALID_ARGUMENT;
	}
	int dim = spPointGetDimension(feature);
	char **pointCoordinates = (char **) malloc(dim * sizeof(*pointCoordinates));
	if (pointCoordinates == NULL) {
		return SP_FEATURES_FILE_API_ALLOC_FAIL;
	}
	for (int i = 0; i < dim; i++) {
		double coordinate = spPointGetAxisCoor(feature, i);
		char *pointCoordinate = (char *) malloc(MAX_FEATURE_COORDINATE_STRING_LEN * sizeof(char));
		int numOfChars = sprintf(pointCoordinate, "%f", coordinate);
		if (pointCoordinate == NULL || numOfChars == 0 || numOfChars > MAX_FEATURE_COORDINATE_STRING_LEN - 1) {
			for (int j = 0; j < i; j++) {
				free(pointCoordinates[i]);
			}
			free(pointCoordinates);
			return SP_FEATURES_FILE_API_INVALID_ARGUMENT;
		}
		pointCoordinates[i] = pointCoordinate;
	}
	SP_FEATURES_FILE_API_MSG returnMsg = SP_FEATURES_FILE_API_SUCCESS;
	char *joinedCoordinates = spUtilStrJoin(pointCoordinates, dim, FEATURE_COORDINATES_DELIM);
	if (joinedCoordinates == NULL) {
		returnMsg = SP_FEATURES_FILE_API_ALLOC_FAIL;
	} else {
		if (fputs(joinedCoordinates, featureFile) == EOF || fputc('\n', featureFile) == EOF) {
			returnMsg = SP_FEATURES_FILE_API_WRITE_ERROR;
		}
	}
	free(joinedCoordinates);
	for (int i = 0; i < dim; i++) {
		free(pointCoordinates[i]);
	}
	free(pointCoordinates);
	return returnMsg;
}

SP_FEATURES_FILE_API_MSG writeFeatures(char *filePath, SPPoint *features, int numOfFeatures) {
	if (numOfFeatures == 0) {
		return SP_FEATURES_FILE_API_INVALID_ARGUMENT;
	}
	char numOfFeaturesAsString[MAX_NUM_OF_FEATURES_STRING_LEN];
	int numberOfChars = sprintf(numOfFeaturesAsString, "%d", numOfFeatures);
	if (numberOfChars == 0 || numberOfChars > MAX_NUM_OF_FEATURES_STRING_LEN - 1) {
		return SP_FEATURES_FILE_API_INVALID_ARGUMENT;
	}
	FILE *featuresFile = fopen(filePath, "w");
	if (featuresFile == NULL) {
		return SP_FEATURES_FILE_API_WRITE_ERROR;
	}

	if (fputs(numOfFeaturesAsString, featuresFile) == EOF || fputc('\n', featuresFile) == EOF) {
		fclose(featuresFile);
		remove(filePath);
		return SP_FEATURES_FILE_API_WRITE_ERROR;
	}

	SP_FEATURES_FILE_API_MSG msg;
	for (int i = 0; i < numOfFeatures; i++) {
		SPPoint feature = features[i];
		msg = writeFeature(featuresFile, feature);
		if (msg != SP_FEATURES_FILE_API_SUCCESS) {
			fclose(featuresFile);
			remove(filePath);
			return msg;
		}
	}
	fclose(featuresFile);
	return SP_FEATURES_FILE_API_SUCCESS;
}

