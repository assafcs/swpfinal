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

/*** Private Methods ***/

/**
 * Loads the number of features from the features file.
 *
 * @param featuresFile The features file stream to load from.
 * @param msg Place-holder for the SP_FEATURES_FILE_API_MSG informing the process result:
 * 		SP_FEATURES_FILE_API_ALLOC_FAIL 		- In case an allocation failure occurred.
 * 		SP_FEATURES_FILE_API_READ_ERROR			- In case reading from the features file went wrong.
 * 		SP_FEATURES_FILE_API_SUCCESS			- In case of success load of number of features.
 *
 * @return
 * 	-1 on non-successful load.
 * 	Otherwise, returns the number of features stored in the features file.
 */
int loadNumberOfFeatures(FILE *featuresFile, SP_FEATURES_FILE_API_MSG *msg) {
	int numberOfFeatures;
	char *numberOfFeaturesAsString = (char *) malloc((MAX_NUM_OF_FEATURES_STRING_LEN + 1) * sizeof(char));

	if (numberOfFeaturesAsString == NULL) {
		*msg = SP_FEATURES_FILE_API_ALLOC_FAIL;
		return -1;
	}
	if (fgets(numberOfFeaturesAsString, MAX_NUM_OF_FEATURES_STRING_LEN, featuresFile) == NULL) {
		free(numberOfFeaturesAsString);
		*msg = SP_FEATURES_FILE_API_READ_ERROR;
		return -1;
	}
	numberOfFeatures = atoi(numberOfFeaturesAsString);

	free(numberOfFeaturesAsString);
	*msg = (numberOfFeatures == 0) ? SP_FEATURES_FILE_API_READ_ERROR : SP_FEATURES_FILE_API_SUCCESS;
	return numberOfFeatures;
}

/**
 * Loads a single feature from the features file stream.
 *
 * @param featuresFile The features file stream to load the feature from
 * @param expectedDimension The expected dimension of the loaded point.
 * @param index The index to be set in the loaded point.
 * @param msg Place-holder for the SP_FEATURES_FILE_API_MSG informing the process result:
 * 		SP_FEATURES_FILE_API_ALLOC_FAIL 		- In case an allocation failure occurred.
 * 		SP_FEATURES_FILE_API_READ_ERROR			- In case reading from the features file went wrong.
 * 		SP_FEATURES_FILE_API_SUCCESS			- In case of success load of number of features.
 *
 * @return
 * 	NULL in case of a non-successful load.
 * 	Otherwise, returns the loaded feature.
 */
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
		*msg = SP_FEATURES_FILE_API_READ_ERROR;
		return NULL;
	}


	splitResult = spUtilStrSplit(featureCoordinatesString, FEATURE_COORDINATES_DELIM, &numberOfCoordinates);
	// The feature coordinates string is no longer needed..
	free(featureCoordinatesString);

	if (splitResult == NULL || numberOfCoordinates != expectedDimension) {
		spUtilFreeStringsArray(splitResult, numberOfCoordinates);
		*msg = SP_FEATURES_FILE_API_READ_ERROR;
		return NULL;
	}

	data = (double *) malloc(numberOfCoordinates * sizeof(double));
	for (int i = 0; i < numberOfCoordinates; i++) {
		double featureCoordinate = atof(splitResult[i]);
		if (featureCoordinate == 0 && strcmp(splitResult[i], "0") != 0) {
			free(data);
			spUtilFreeStringsArray(splitResult, numberOfCoordinates);
			*msg = SP_FEATURES_FILE_API_READ_ERROR;
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

/**
 * Writes the number of features to the features file.
 *
 * @param featuresFile The features file stream to write the number of features to.
 * @param numOfFeatures The number of features to write.
 *
 * @return
 * 	SP_FEATURES_FILE_API_MSG informing the process result:
 * 		SP_FEATURES_FILE_API_INVALID_ARGUMENT		- In case the numOfFeatures base 10 representation is larger than MAX_NUM_OF_FEATURES_STRING_LEN digits.
 * 		SP_FEATURES_FILE_API_WRITE_ERROR			- In case writing to the features file went wrong.
 * 		SP_FEATURES_FILE_API_SUCCESS				- In case of successful write.
 *
 */
SP_FEATURES_FILE_API_MSG writeNumberOfFeatures(FILE* featuresFile, int numOfFeatures) {
	char numOfFeaturesAsString[MAX_NUM_OF_FEATURES_STRING_LEN];
	int numberOfChars = sprintf(numOfFeaturesAsString, "%d", numOfFeatures);
	if (numberOfChars == 0 || numberOfChars > MAX_NUM_OF_FEATURES_STRING_LEN - 1) {
		return SP_FEATURES_FILE_API_INVALID_ARGUMENT;
	}

	if (fputs(numOfFeaturesAsString, featuresFile) == EOF || fputc('\n', featuresFile) == EOF) {
		return SP_FEATURES_FILE_API_WRITE_ERROR;
	}
	return SP_FEATURES_FILE_API_SUCCESS;
}

/**
 * Writes a single feature to the features file.
 *
 * @param featuresFile The features file stream to the write the feature to.
 * @param feature The SPPoint representing the feature to write.
 *
 * @return
 * 	SP_FEATURES_FILE_API_MSG informing the process result:
 * 		SP_FEATURES_FILE_API_INVALID_ARGUMENT		- In case a feature coordinate base 10 representation is larger than MAX_FEATURE_COORDINATE_STRING_LEN digits.
 * 		SP_FEATURES_FILE_API_WRITE_ERROR			- In case writing to the features file went wrong.
 * 		SP_FEATURES_FILE_API_ALLOC_FAIL				- In case of allocation failure.
 * 		SP_FEATURES_FILE_API_SUCCESS				- In case of successful write.
 */
SP_FEATURES_FILE_API_MSG writeFeature(FILE* featureFile, SPPoint feature) {
	int i, numOfChars, dim = spPointGetDimension(feature);
	char *pointCoordinate = NULL, *joinedCoordinates = NULL;
	double coordinate;
	SP_FEATURES_FILE_API_MSG returnMsg;
	char **pointCoordinates = (char **) malloc(dim * sizeof(*pointCoordinates));
	if (pointCoordinates == NULL) {
		return SP_FEATURES_FILE_API_ALLOC_FAIL;
	}
	for (i = 0; i < dim; i++) {
		coordinate = spPointGetAxisCoor(feature, i);
		pointCoordinate = (char *) malloc(MAX_FEATURE_COORDINATE_STRING_LEN * sizeof(char));
		numOfChars = sprintf(pointCoordinate, "%f", coordinate);
		if (pointCoordinate == NULL || numOfChars == 0 || numOfChars > MAX_FEATURE_COORDINATE_STRING_LEN - 1) {
			spUtilFreeStringsArray(pointCoordinates, i);
			free(pointCoordinate);
			return SP_FEATURES_FILE_API_INVALID_ARGUMENT;
		}
		pointCoordinates[i] = pointCoordinate;
	}
	returnMsg = SP_FEATURES_FILE_API_SUCCESS;
	joinedCoordinates = spUtilStrJoin((const char **)pointCoordinates, dim, FEATURE_COORDINATES_DELIM);
	if (joinedCoordinates == NULL) {
		returnMsg = SP_FEATURES_FILE_API_ALLOC_FAIL;
	} else {
		if (fputs(joinedCoordinates, featureFile) == EOF || fputc('\n', featureFile) == EOF) {
			returnMsg = SP_FEATURES_FILE_API_WRITE_ERROR;
		}
	}
	free(joinedCoordinates);
	spUtilFreeStringsArray(pointCoordinates, dim);
	return returnMsg;
}

SPPoint *spFeaturesFileAPILoad(const char *filePath, int index, int expectedFeatureDimension, int *numOfFeaturesLoaded,
		SP_FEATURES_FILE_API_MSG *msg) {
	if (filePath == NULL || numOfFeaturesLoaded == NULL || msg == NULL || expectedFeatureDimension <= 0) {
		*msg = SP_FEATURES_FILE_API_INVALID_ARGUMENT;
		return NULL;
	}
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

SP_FEATURES_FILE_API_MSG spFeaturesFileAPIWrite(const char *filePath, const SPPoint *features, int numOfFeatures) {
	int i;
	FILE *featuresFile;
	SP_FEATURES_FILE_API_MSG msg;

	if (filePath == NULL || features == NULL || numOfFeatures <= 0) {
		return SP_FEATURES_FILE_API_INVALID_ARGUMENT;
	}
	featuresFile = fopen(filePath, "w");
	if (featuresFile == NULL) {
		return SP_FEATURES_FILE_API_WRITE_ERROR;
	}

	msg = writeNumberOfFeatures(featuresFile, numOfFeatures);
	if (msg != SP_FEATURES_FILE_API_SUCCESS) {
		fclose(featuresFile);
		remove(filePath);
		return msg;
	}

	for (i = 0; i < numOfFeatures; i++) {
		msg = writeFeature(featuresFile, features[i]);
		if (msg != SP_FEATURES_FILE_API_SUCCESS) {
			fclose(featuresFile);
			remove(filePath);
			return msg;
		}
	}
	fclose(featuresFile);
	return SP_FEATURES_FILE_API_SUCCESS;
}

