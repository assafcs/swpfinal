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

const SP_SEARCH_TREE_CREATION_MSG CREATE_MSG_FOR_FEATURE_API_MSG[] = {
	SP_SEARCH_TREE_CREATION_INVALID_ARGUMENT,
	SP_SEARCH_TREE_CREATION_FEATURE_FILE_MISSING,
	SP_SEARCH_TREE_CREATION_ALLOC_FAIL,
	SP_SEARCH_TREE_CREATION_WRITE_ERROR,
	SP_SEARCH_TREE_CREATION_LOAD_ERROR,
	SP_SEARCH_TREE_CREATION_SUCCESS
};


void destroyVariables(SPPoint *allFeatures, int totalFeaturesCount, char *imagePath, char *featuresPath) {
	for (int j = 0; j < totalFeaturesCount; j++) {
		spPointDestroy(allFeatures[j]);
	}
	free(allFeatures);
	free(imagePath);
	free(featuresPath);
}

SPPoint *loadAllFeatures(SPConfig config, int *numberOfFeatures, SP_SEARCH_TREE_CREATION_MSG *msg) {
	char *featuresPath = NULL;
	SPPoint *allFeatures = NULL;

	SP_CONFIG_MSG resultMSG;
	int numOfImages = spConfigGetNumOfImages(config, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_SEARCH_TREE_CREATION_CONFIG_ERROR;
		return NULL;
	}

	int expectedDimension = spConfigGetPCADim(config, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_SEARCH_TREE_CREATION_CONFIG_ERROR;
		return NULL;
	}

	allFeatures = (SPPoint *) malloc(1 * sizeof(SPPoint));
	featuresPath = (char *) malloc (MAX_PATH_LENGTH * sizeof(char));
	if (allFeatures == NULL || featuresPath == NULL) {
		destroyVariables(allFeatures, 0, NULL, featuresPath);
		*msg = SP_SEARCH_TREE_CREATION_ALLOC_FAIL;
		return NULL;
	}

	int numOfFeaturesLoaded;
	int totalFeaturesCount = 0;

	for (int imageIndex = 0; imageIndex < numOfImages; imageIndex++) {
		if (spConfigGetImageFeaturesPath(featuresPath, config, imageIndex) != SP_CONFIG_SUCCESS) {
			destroyVariables(allFeatures, totalFeaturesCount, NULL, featuresPath);
			*msg = SP_SEARCH_TREE_CREATION_CONFIG_ERROR;
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
			*msg = SP_SEARCH_TREE_CREATION_ALLOC_FAIL;
			return NULL;
		}

		for (int i = 0; i < numOfFeaturesLoaded; i++) {
			allFeatures[totalFeaturesCount - numOfFeaturesLoaded + i] = features[i];
		}
	}

	free(featuresPath);
	*numberOfFeatures = totalFeaturesCount;
	*msg = SP_SEARCH_TREE_CREATION_SUCCESS;
	return allFeatures;
}


SPPoint *extractAllFeatures(SPConfig config, int *numberOfFeatures, SP_SEARCH_TREE_CREATION_MSG *msg,
		 FeatureExractionFunction featureExactionFunction) {
	char *imagePath = NULL, *featuresPath = NULL;
	SPPoint *allFeatures = NULL;

	SP_CONFIG_MSG resultMSG;
	int numOfImages = spConfigGetNumOfImages(config, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_SEARCH_TREE_CREATION_CONFIG_ERROR;
		return NULL;
	}

	allFeatures = (SPPoint *) malloc(1 * sizeof(SPPoint));
	imagePath = (char *) malloc (MAX_PATH_LENGTH * sizeof(char));
	featuresPath = (char *) malloc (MAX_PATH_LENGTH * sizeof(char));
	if (allFeatures == NULL || imagePath == NULL || featuresPath == NULL) {
		destroyVariables(allFeatures, 0, imagePath, featuresPath);
		*msg = SP_SEARCH_TREE_CREATION_ALLOC_FAIL;
		return NULL;
	}
	int numOfFeaturesExtracted;
	int totalFeaturesCount = 0;
	for (int imageIndex = 0; imageIndex < numOfImages; imageIndex++) {
		if (spConfigGetImagePath(imagePath, config, imageIndex) != SP_CONFIG_SUCCESS ||
				spConfigGetImageFeaturesPath(featuresPath, config, imageIndex) != SP_CONFIG_SUCCESS) {
			destroyVariables(allFeatures, totalFeaturesCount, imagePath, featuresPath);
			*msg = SP_SEARCH_TREE_CREATION_CONFIG_ERROR;
			return NULL;
		}
		SPPoint *features = featureExactionFunction(imagePath, imageIndex, &numOfFeaturesExtracted);
		if (features == NULL || numOfFeaturesExtracted <= 0) {
			destroyVariables(allFeatures, totalFeaturesCount, imagePath, featuresPath);
			*msg = SP_SEARCH_TREE_CREATION_FEATURES_EXTRACTION_ERROR;
			return NULL;
		}

		totalFeaturesCount += numOfFeaturesExtracted;
		SP_FEATURES_FILE_API_MSG featuresFileAPIMsg = spFeaturesFileAPIWrite(featuresPath, features, numOfFeaturesExtracted);

		if (featuresFileAPIMsg != SP_FEATURES_FILE_API_SUCCESS) {
			destroyVariables(allFeatures, totalFeaturesCount, imagePath, featuresPath);
			spKDArrayFreePointsArray(features, numOfFeaturesExtracted);
			*msg = CREATE_MSG_FOR_FEATURE_API_MSG[featuresFileAPIMsg];
			return NULL;
		}

		allFeatures = (SPPoint *) realloc(allFeatures, totalFeaturesCount * sizeof(SPPoint));
		if (allFeatures == NULL) {
			destroyVariables(allFeatures, totalFeaturesCount, imagePath, featuresPath);
			spKDArrayFreePointsArray(features, numOfFeaturesExtracted);
			*msg = SP_SEARCH_TREE_CREATION_ALLOC_FAIL;
			return NULL;
		}

		for (int i = 0; i < numOfFeaturesExtracted; i++) {
			allFeatures[totalFeaturesCount - numOfFeaturesExtracted + i] = features[i];
		}
	}

	free(imagePath);
	free(featuresPath);
	*numberOfFeatures = totalFeaturesCount;
	*msg = SP_SEARCH_TREE_CREATION_SUCCESS;
	return allFeatures;
}


SPPoint *getAllFeatures(SPConfig config, int *numberOfFeatures, SP_SEARCH_TREE_CREATION_MSG *msg,
		FeatureExractionFunction featureExtractionFunction) {
	if (config == NULL) {
		return NULL;
	}
	SP_CONFIG_MSG resultMSG;
	bool extractionMode = spConfigIsExtractionMode(config, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_SEARCH_TREE_CREATION_CONFIG_ERROR;
		return NULL;
	}
	return extractionMode ? extractAllFeatures(config, numberOfFeatures, msg, featureExtractionFunction) :
			loadAllFeatures(config, numberOfFeatures, msg);
}

SPKDTreeNode spImagesSearchTreeCreate(const SPConfig config,
		FeatureExractionFunction featureExtractionFunction,
		SP_SEARCH_TREE_CREATION_MSG *msg) {
	SPPoint *allFeatures = NULL;

	int totalFeaturesCount;
	allFeatures = getAllFeatures(config, &totalFeaturesCount, msg, featureExtractionFunction);
	if (allFeatures == NULL || totalFeaturesCount <= 0 || *msg != SP_SEARCH_TREE_CREATION_SUCCESS) {
		destroyVariables(allFeatures, totalFeaturesCount, NULL, NULL);
		return NULL;
	}

	SPKDArray kdArray = spKDArrayInit(allFeatures, totalFeaturesCount);
	if (kdArray == NULL) {
		destroyVariables(allFeatures, totalFeaturesCount, NULL, NULL);
		return NULL;
	}
	SP_CONFIG_MSG configMsg;
	SP_TREE_SPLIT_METHOD splitMethod = spConfigGetSplitMethod(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS) {
		spKDArrayDestroy(kdArray);
		destroyVariables(allFeatures, totalFeaturesCount, NULL, NULL);
		return NULL;
	}
	SPKDTreeNode tree = spKDTreeBuild(kdArray, splitMethod);
	if (tree == NULL) {
		spKDArrayDestroy(kdArray);
		destroyVariables(allFeatures, totalFeaturesCount, NULL, NULL);
		return NULL;
	}
	return tree;
}
