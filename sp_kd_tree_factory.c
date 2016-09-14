/*
 * sp_kd_tree_factory.c
 *
 *  Created on: Sep 11, 2016
 *      Author: mataneilat
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sp_features_file_api.h"
#include "SPKDArray.h"
#include "sp_kd_tree_factory.h"

/** Mapping to wrap features file API message with kd-tree creation message. */
const SP_KD_TREE_CREATION_MSG CREATE_MSG_FOR_FEATURE_API_MSG[] = {
	SP_KD_TREE_CREATION_INVALID_ARGUMENT,
	SP_KD_TREE_CREATION_FEATURE_FILE_MISSING,
	SP_KD_TREE_CREATION_ALLOC_FAIL,
	SP_KD_TREE_CREATION_WRITE_ERROR,
	SP_KD_TREE_CREATION_LOAD_ERROR,
	SP_KD_TREE_CREATION_SUCCESS
};

/*** Private Methods ***/

/**
 * Deallocates the given variables.
 *
 * @param allFeatures A features array to be deallocated.
 * @param totalFeaturesCount The number of features in the features array.
 * @param imagePath String representing an image path to be deallocated.
 * @param featuresPath String representing a feature file path to be deallocated.
 *
 */
void destroyVariables(SPPoint *allFeatures, int totalFeaturesCount, char *imagePath, char *featuresPath) {
	int j;
	for (j = 0; j < totalFeaturesCount; j++) {
		spPointDestroy(allFeatures[j]);
	}
	free(allFeatures);
	free(imagePath);
	free(featuresPath);
}

/**
 * Loads the features for the configured images, as one array of features.
 *
 * @param config The configuration used to load images features.
 * @param numberOfFeatures Place-holder for the total number of features loaded.
 * @param msg SP_KD_TREE_CREATION_MSG informing the load result:
 * 		SP_KD_TREE_CREATION_CONFIG_ERROR			- In case of a configuration access error.
 * 		SP_KD_TREE_CREATION_ALLOC_FAIL				- In case of allocation failure.
 *		SP_KD_TREE_CREATION_FEATURE_FILE_MISSING 	- In case a features file was missing
 *		SP_KD_TREE_CREATION_LOAD_ERROR				- In case loading features from a file went wrong.
 *		SP_KD_TREE_CREATION_SUCCESS					- In case all features were successfully loaded.
 *
 * @return
 * 	NULL in case of a non-successful load.
 * 	Otherwise, returns the loaded features.
 */
SPPoint *loadAllFeatures(SPConfig config, int *numberOfFeatures, SP_KD_TREE_CREATION_MSG *msg) {
	char *featuresPath = NULL;
	SPPoint *allFeatures = NULL;
	SP_CONFIG_MSG resultMSG;
	int i, numOfFeaturesLoaded, expectedDimension, imageIndex, totalFeaturesCount = 0,
			numOfImages = spConfigGetNumOfImages(config, &resultMSG);

	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_KD_TREE_CREATION_CONFIG_ERROR;
		return NULL;
	}

	expectedDimension = spConfigGetPCADim(config, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_KD_TREE_CREATION_CONFIG_ERROR;
		return NULL;
	}

	allFeatures = (SPPoint *) malloc(1 * sizeof(SPPoint));
	featuresPath = (char *) malloc (MAX_PATH_LENGTH * sizeof(char));
	if (allFeatures == NULL || featuresPath == NULL) {
		destroyVariables(allFeatures, 0, NULL, featuresPath);
		*msg = SP_KD_TREE_CREATION_ALLOC_FAIL;
		return NULL;
	}

	for (imageIndex = 0; imageIndex < numOfImages; imageIndex++) {
		if (spConfigGetImageFeaturesPath(featuresPath, config, imageIndex) != SP_CONFIG_SUCCESS) {
			destroyVariables(allFeatures, totalFeaturesCount, NULL, featuresPath);
			*msg = SP_KD_TREE_CREATION_CONFIG_ERROR;
			return NULL;
		}
		SP_FEATURES_FILE_API_MSG featuresAPIMsg;
		SPPoint *features = spFeaturesFileAPILoad(featuresPath, imageIndex, expectedDimension, &numOfFeaturesLoaded, &featuresAPIMsg);
		if (featuresAPIMsg != SP_FEATURES_FILE_API_SUCCESS) {
			destroyVariables(allFeatures, totalFeaturesCount, NULL, featuresPath);
			*msg = CREATE_MSG_FOR_FEATURE_API_MSG[featuresAPIMsg];
			return NULL;
		}

		totalFeaturesCount += numOfFeaturesLoaded;

		allFeatures = (SPPoint *) realloc(allFeatures, totalFeaturesCount * sizeof(SPPoint));
		if (allFeatures == NULL) {
			destroyVariables(allFeatures, totalFeaturesCount, NULL, featuresPath);
			spKDArrayFreePointsArray(features, numOfFeaturesLoaded);
			*msg = SP_KD_TREE_CREATION_ALLOC_FAIL;
			return NULL;
		}

		for (i = 0; i < numOfFeaturesLoaded; i++) {
			allFeatures[totalFeaturesCount - numOfFeaturesLoaded + i] = features[i];
		}
	}

	free(featuresPath);
	*numberOfFeatures = totalFeaturesCount;
	*msg = SP_KD_TREE_CREATION_SUCCESS;
	return allFeatures;
}

/**
 * Extracts features for the configured images, as one array of features.
 * For each image, this method also writes the extracted features to .feats file using sp_features_file_api.
 *
 * @param config The configuration used to extract images features.
 * @param numberOfFeatures Place-holder for the total number of extracted features.
 * @param msg SP_KD_TREE_CREATION_MSG informing the extraction result:
 * 		SP_KD_TREE_CREATION_CONFIG_ERROR				- In case of a configuration access error.
 * 		SP_KD_TREE_CREATION_ALLOC_FAIL					- In case of allocation failure.
 *		SP_KD_TREE_CREATION_WRITE_ERROR					- In case writing features to a file went wrong.
 *		SP_KD_TREE_CREATION_FEATURES_EXTRACTION_ERROR	- In case features extraction went wrong.
 *		SP_KD_TREE_CREATION_SUCCESS						- In case all features were successfully extracted and written.
 * @param featureExactionFunction The function used to extract image features.
 *
 * @return
 * 	NULL in case of a non-successful extraction/write.
 * 	Otherwise, returns the extracted features an one array.
 */
SPPoint *extractAllFeatures(SPConfig config, int *numberOfFeatures, SP_KD_TREE_CREATION_MSG *msg,
		 FeatureExractionFunction featureExactionFunction) {
	char *imagePath = NULL, *featuresPath = NULL;
	SPPoint *allFeatures = NULL, *features = NULL;
	SP_CONFIG_MSG resultMSG;
	SP_FEATURES_FILE_API_MSG featuresFileAPIMsg;
	int i, imageIndex, numOfFeaturesExtracted, totalFeaturesCount = 0, numOfImages = spConfigGetNumOfImages(config, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_KD_TREE_CREATION_CONFIG_ERROR;
		return NULL;
	}

	allFeatures = (SPPoint *) malloc(1 * sizeof(SPPoint));
	imagePath = (char *) malloc (MAX_PATH_LENGTH * sizeof(char));
	featuresPath = (char *) malloc (MAX_PATH_LENGTH * sizeof(char));
	if (allFeatures == NULL || imagePath == NULL || featuresPath == NULL) {
		destroyVariables(allFeatures, 0, imagePath, featuresPath);
		*msg = SP_KD_TREE_CREATION_ALLOC_FAIL;
		return NULL;
	}

	for (imageIndex = 0; imageIndex < numOfImages; imageIndex++) {
		if (spConfigGetImagePath(imagePath, config, imageIndex) != SP_CONFIG_SUCCESS ||
				spConfigGetImageFeaturesPath(featuresPath, config, imageIndex) != SP_CONFIG_SUCCESS) {
			destroyVariables(allFeatures, totalFeaturesCount, imagePath, featuresPath);
			*msg = SP_KD_TREE_CREATION_CONFIG_ERROR;
			return NULL;
		}
		features = featureExactionFunction(imagePath, imageIndex, &numOfFeaturesExtracted);
		if (features == NULL || numOfFeaturesExtracted <= 0) {
			destroyVariables(allFeatures, totalFeaturesCount, imagePath, featuresPath);
			*msg = SP_KD_TREE_CREATION_FEATURES_EXTRACTION_ERROR;
			return NULL;
		}

		featuresFileAPIMsg = spFeaturesFileAPIWrite(featuresPath, features, numOfFeaturesExtracted);

		if (featuresFileAPIMsg != SP_FEATURES_FILE_API_SUCCESS) {
			destroyVariables(allFeatures, totalFeaturesCount, imagePath, featuresPath);
			spKDArrayFreePointsArray(features, numOfFeaturesExtracted);
			*msg = CREATE_MSG_FOR_FEATURE_API_MSG[featuresFileAPIMsg];
			return NULL;
		}

		totalFeaturesCount += numOfFeaturesExtracted;
		allFeatures = (SPPoint *) realloc(allFeatures, totalFeaturesCount * sizeof(SPPoint));
		if (allFeatures == NULL) {
			destroyVariables(allFeatures, totalFeaturesCount, imagePath, featuresPath);
			spKDArrayFreePointsArray(features, numOfFeaturesExtracted);
			*msg = SP_KD_TREE_CREATION_ALLOC_FAIL;
			return NULL;
		}

		for (i = 0; i < numOfFeaturesExtracted; i++) {
			allFeatures[totalFeaturesCount - numOfFeaturesExtracted + i] = features[i];
		}
		free(features);
	}

	free(imagePath);
	free(featuresPath);
	*numberOfFeatures = totalFeaturesCount;
	*msg = SP_KD_TREE_CREATION_SUCCESS;
	return allFeatures;
}


/**
 * According to the configured extraction-mode, either extract or load all of the images features.
 *
 * @param config The configuration used to extract/load images features.
 * @param numberOfFeatures Place-holder for the total number of extracted/loaded features.
 * @param msg SP_KD_TREE_CREATION_MSG informing the extraction result:
 * 		SP_KD_TREE_CREATION_CONFIG_ERROR				- In case of a configuration access error.
 * 		SP_KD_TREE_CREATION_ALLOC_FAIL					- In case of allocation failure.
 * 		SP_KD_TREE_CREATION_FEATURE_FILE_MISSING 		- In case a features file was missing
 *		SP_KD_TREE_CREATION_LOAD_ERROR					- In case loading features from a file went wrong.
 *		SP_KD_TREE_CREATION_WRITE_ERROR					- In case writing features to a file went wrong.
 *		SP_KD_TREE_CREATION_FEATURES_EXTRACTION_ERROR	- In case features extraction went wrong.
 *		SP_KD_TREE_CREATION_SUCCESS						- In case all features were successfully extracted and written.
 * @param featureExactionFunction The function used to extract image features if needed.
 *
 * @return
 * 	NULL in case of a non-successful extraction+write/load
 * 	Otherwise, returns the extracted/loaded images features.
 */
SPPoint *getAllFeatures(SPConfig config, int *numberOfFeatures, SP_KD_TREE_CREATION_MSG *msg,
		FeatureExractionFunction featureExtractionFunction) {
	if (config == NULL) {
		return NULL;
	}
	SP_CONFIG_MSG resultMSG;
	bool extractionMode = spConfigIsExtractionMode(config, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_KD_TREE_CREATION_CONFIG_ERROR;
		return NULL;
	}
	return extractionMode ? extractAllFeatures(config, numberOfFeatures, msg, featureExtractionFunction) :
			loadAllFeatures(config, numberOfFeatures, msg);
}

/*** Public Methods ***/

SPKDTreeNode spImagesKDTreeCreate(const SPConfig config,
		FeatureExractionFunction featureExtractionFunction,
		SP_KD_TREE_CREATION_MSG *msg) {

	SPPoint *allFeatures = NULL;
	int totalFeaturesCount = 0;
	SPKDArray kdArray;
	SP_CONFIG_MSG configMsg;
	SP_TREE_SPLIT_METHOD splitMethod;
	SPKDTreeNode tree;
	if (config == NULL || featureExtractionFunction == NULL || msg == NULL) {
		*msg = SP_KD_TREE_CREATION_INVALID_ARGUMENT;
		return NULL;
	}
	allFeatures = getAllFeatures(config, &totalFeaturesCount, msg, featureExtractionFunction);
	if (allFeatures == NULL || totalFeaturesCount <= 0 || *msg != SP_KD_TREE_CREATION_SUCCESS) {
		destroyVariables(allFeatures, totalFeaturesCount, NULL, NULL);
		return NULL;
	}
	kdArray = spKDArrayInit(allFeatures, totalFeaturesCount);
	if (kdArray == NULL) {
		destroyVariables(allFeatures, totalFeaturesCount, NULL, NULL);
		return NULL;
	}
	splitMethod = spConfigGetSplitMethod(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS) {
		spKDArrayDestroy(kdArray);
		destroyVariables(allFeatures, totalFeaturesCount, NULL, NULL);
		return NULL;
	}
	tree = spKDTreeBuild(kdArray, splitMethod);
	if (tree == NULL) {
		spKDArrayDestroy(kdArray);
		destroyVariables(allFeatures, totalFeaturesCount, NULL, NULL);
		return NULL;
	}
	spKDArrayDestroy(kdArray);
	destroyVariables(allFeatures, totalFeaturesCount, NULL, NULL);
	return tree;
}
