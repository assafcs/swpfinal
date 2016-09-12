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

typedef struct hit_info_t {
   int index;
   int hits;
} HitInfo;

int cmpHitInfos(const void * a, const void * b) {
	HitInfo aInfo = *(HitInfo*)a;
	HitInfo bInfo = *(HitInfo*)b;
	int res = (bInfo.hits - aInfo.hits);
	if (res != 0) {
		return res;
	}
	return aInfo.index - bInfo.index;
}

int *spFindSimilarImagesIndices(const SPConfig config, const char *queryImagePath,
		const SPKDTreeNode searchTree, int *resultsCount, FeatureExractionFunction extractionFunc) {
	if (config == NULL || searchTree == NULL || resultsCount == NULL) {
		return NULL;
	}
	SP_CONFIG_MSG configMsg;
	int KNN = spConfigGetKNN(config, &configMsg);

	if (configMsg != SP_CONFIG_SUCCESS) {
		return NULL;
	}
	int numOfImages = spConfigGetNumOfImages(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS) {
		return NULL;
	}
	int similarImages = spConfigGetNumOfSimilarImages(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS) {
		return NULL;
	}

	HitInfo* hitInfos = (HitInfo*) malloc(numOfImages * sizeof(HitInfo));
	if (hitInfos == NULL) {
		return NULL;
	}

	for (int i = 0; i < numOfImages; i++) {
		HitInfo info = (HitInfo) {i, 0};
		hitInfos[i] = info;
	}

	int numOfFeaturesExtracted;

	SPPoint *features = extractionFunc(queryImagePath, 0, &numOfFeaturesExtracted);

	if (features == NULL || numOfFeaturesExtracted <= 0) {
		free(hitInfos);
		return NULL;
	}

	SPBPQueue queue = spBPQueueCreate(KNN);
	for (int i = 0; i < numOfFeaturesExtracted; i++) {
		SPPoint feature = features[i];
		spKNearestNeighbours(searchTree, queue, feature);
		int queueSize = spBPQueueSize(queue);
		for (int j = 0; j < queueSize; j++) {
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

	int* resValue = (int *) malloc(similarImages * sizeof(int));

	if (resValue == NULL) {
		free(hitInfos);
		return NULL;
	}

	for (int i = 0; i < similarImages; i++) {
		resValue[i] = hitInfos[i].index;
	}

	free(hitInfos);
	*resultsCount = similarImages;
	return resValue;

}
