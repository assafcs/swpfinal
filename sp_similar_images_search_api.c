/*
 * sp_similar_images_search_api.c
 *
 *  Created on: Sep 12, 2016
 *      Author: mataneilat
 */

#include "sp_similar_images_search_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SPBPriorityQueue.h"
#include "sp_algorithms.h"
#include "SPKDArray.h"
#include "sp_util.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"

/** Structure to count images 'hits', meaning the amount of times an image's feature was found in nearest neighbor search. */
typedef struct hit_info_t {
   int index;
   int hits;
} HitInfo;


/*** Private Methods ***/

/**
 * Compares to HitInfos - to be used by qsort.
 * First it compares by hits in descending order - meaning higher amount of hits will come before.
 * If two HitInfos have same amount of hits, than the one with lower index will come before.
 *
 * @param a - First HitInfo pointer.
 * @param b - Second HitInfo pointer.
 *
 * @return
 * 	The compare result.
 */
int cmpHitInfos(const void * a, const void * b) {
	HitInfo aInfo = *(HitInfo*)a;
	HitInfo bInfo = *(HitInfo*)b;
	int res = (bInfo.hits - aInfo.hits);
	if (res != 0) {
		return res;
	}
	return aInfo.index - bInfo.index;
}

/**
 * Deallocates variables used in the query method.
 *
 * @param features Features array to destroy.
 * @param numOfFeatures The number of features in the array.
 * @param hitInfos Array of HitInfos to free.
 *
 */
void destroyImageQueryVariables(SPPoint *features, int numOfFeatures, HitInfo *hitInfos) {
	free(hitInfos);
	spKDArrayFreePointsArray(features, numOfFeatures);
}

/*** Public Methods ***/

int *spFindSimilarImagesIndices(const SPConfig config, const char *queryImagePath,
		const SPKDTreeNode searchTree, int *resultsCount, FeatureExractionFunction extractionFunc,
		SP_SIMILAR_IMAGES_SEARCH_API_MSG *msg) {
	SP_CONFIG_MSG configMsg;
	int i, j, KNN, numOfImages, similarImages, numOfFeaturesExtracted, *resValue;
	SPPoint *features;
	HitInfo* hitInfos;
	if (config == NULL || queryImagePath == NULL || searchTree == NULL || resultsCount == NULL || extractionFunc == NULL) {
		*msg = SP_SIMILAR_IMAGES_SEARCH_API_INVALID_ARGUMENT;
		return NULL;
	}

	KNN = spConfigGetKNN(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS) {
		*msg = SP_SIMILAR_IMAGES_SEARCH_API_CONFIG_ERROR;
		return NULL;
	}
	numOfImages = spConfigGetNumOfImages(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS) {
		*msg = SP_SIMILAR_IMAGES_SEARCH_API_CONFIG_ERROR;
		return NULL;
	}
	similarImages = spConfigGetNumOfSimilarImages(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS) {
		*msg = SP_SIMILAR_IMAGES_SEARCH_API_CONFIG_ERROR;
		return NULL;
	}

	hitInfos = (HitInfo*) malloc(numOfImages * sizeof(HitInfo));
	if (hitInfos == NULL) {
		spLoggerPrintError(ALLOCATION_ERROR_MSG, __FILE__, __func__, __LINE__);
		*msg = SP_SIMILAR_IMAGES_SEARCH_API_ALLOC_FAIL;
		return NULL;
	}

	for (i = 0; i < numOfImages; i++) {
		HitInfo info = (HitInfo) {i, 0};
		hitInfos[i] = info;
	}

	features = extractionFunc(queryImagePath, 0, &numOfFeaturesExtracted);

	if (features == NULL || numOfFeaturesExtracted <= 0) {
		destroyImageQueryVariables(features, numOfFeaturesExtracted, hitInfos);
		*msg = SP_SIMILAR_IMAGES_SEARCH_API_FEATURES_EXTRACTION_ERROR;
		return NULL;
	}

	SPBPQueue queue = spBPQueueCreate(KNN);
	if (queue == NULL) {
		spLoggerPrintError(ALLOCATION_ERROR_MSG, __FILE__, __func__, __LINE__);
		destroyImageQueryVariables(features, numOfFeaturesExtracted, hitInfos);
		*msg = SP_SIMILAR_IMAGES_SEARCH_API_ALLOC_FAIL;
		return NULL;
	}

	for (i = 0; i < numOfFeaturesExtracted; i++) {
		SPPoint feature = features[i];
		spKNearestNeighbours(searchTree, queue, feature);
		int queueSize = spBPQueueSize(queue);
		for (j = 0; j < queueSize; j++) {
			SPListElement listElement = spBPQueuePeek(queue);
			int index = spListElementGetIndex(listElement);
			hitInfos[index].hits++;
			spBPQueueDequeue(queue);
			spListElementDestroy(listElement);
		}
		spBPQueueClear(queue);
	}
	// No need for the queue anymore
	spBPQueueDestroy(queue);

	qsort(hitInfos, numOfImages, sizeof(HitInfo), cmpHitInfos);

	resValue = (int *) malloc(similarImages * sizeof(int));

	if (resValue == NULL) {
		spLoggerPrintError(ALLOCATION_ERROR_MSG, __FILE__, __func__, __LINE__);
		destroyImageQueryVariables(features, numOfFeaturesExtracted, hitInfos);
		*msg = SP_SIMILAR_IMAGES_SEARCH_API_ALLOC_FAIL;
		return NULL;
	}

	for (i = 0; i < similarImages; i++) {
		resValue[i] = hitInfos[i].index;
	}

	destroyImageQueryVariables(features, numOfFeaturesExtracted, hitInfos);
	*resultsCount = similarImages;
	*msg = SP_SIMILAR_IMAGES_SEARCH_API_SUCCESS;
	return resValue;

}
