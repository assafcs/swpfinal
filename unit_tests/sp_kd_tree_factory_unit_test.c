/*
 * sp_kd_tree_factory_unit_test.c
 *
 *  Created on: Sep 14, 2016
 *      Author: mataneilat
 */

#include "../SPPoint.h"
#include "../SPConfig.h"
#include "../SPKDTree.h"
#include "../sp_kd_tree_factory.h"
#include "string.h"
#include <stdlib.h>
#include <stdbool.h>
#include "unit_test_util.h"
#include "common_test_util.h"

SPPoint *extractionMockFunction(const char *imagePath, int imageIndex, int *numOfFacturesExtracted) {
	SPPoint *points;
	if (strcmp(imagePath, "./test_resources/sp0.img")) {
		points = (SPPoint *) malloc(2 * sizeof(*points));
		points[0] = indexedThreeDPoint(imageIndex, 1, 60, -5.5);
		points[1] = indexedThreeDPoint(imageIndex, 123, 70, -4.5);
		*numOfFacturesExtracted = 2;
		return points;
	} else if (strcmp(imagePath, "./test_resources/sp1.img")) {
		points = (SPPoint *) malloc(2 * sizeof(*points));
		points[0] = indexedThreeDPoint(imageIndex, 2, 80, 4.5);
		points[1] = indexedThreeDPoint(imageIndex, 9, 140.5, 7.5);
		*numOfFacturesExtracted = 2;
		return points;
	} else if (strcmp(imagePath, "./test_resources/sp2.img")) {
		points = (SPPoint *) malloc(sizeof(*points));
		points[0] = indexedThreeDPoint(imageIndex, 3, 8, 133.5);
		*numOfFacturesExtracted = 2;
		return points;
	}
	*numOfFacturesExtracted = 0;
	return NULL;

}

static bool kdTreeFactoryCreationTest() {
	SP_CONFIG_MSG configMsg;
	SPConfig config = spConfigCreate("./test_resources/tree_factory_test_config.txt", &configMsg);
	ASSERT_SAME(configMsg, SP_CONFIG_SUCCESS);
	ASSERT_NOT_NULL(config);

	SP_KD_TREE_CREATION_MSG treeCreationMsg;
	SPKDTreeNode searchTree = spImagesKDTreeCreate(config, extractionMockFunction, &treeCreationMsg);

	ASSERT_NOT_NULL(searchTree);
	ASSERT_SAME(treeCreationMsg, SP_KD_TREE_CREATION_SUCCESS);

	spKDTreeDestroy(searchTree);
	spConfigDestroy(config);
	return true;
}

int main() {
	printf("Running SPKDTreeFactoryTest.. \n");
	RUN_TEST(kdTreeFactoryCreationTest);
}
