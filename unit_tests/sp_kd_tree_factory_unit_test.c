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
		points[0] = nDPoint(imageIndex, 10, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0);
		points[1] = nDPoint(imageIndex, 10, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0);
		*numOfFacturesExtracted = 2;
		return points;
	} else if (strcmp(imagePath, "./test_resources/sp1.img")) {
		points = (SPPoint *) malloc(2 * sizeof(*points));
		points[0] = nDPoint(imageIndex, 10, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0);
		points[1] = nDPoint(imageIndex, 10, 31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0);
		*numOfFacturesExtracted = 2;
		return points;
	} else if (strcmp(imagePath, "./test_resources/sp2.img")) {
		points = (SPPoint *) malloc(sizeof(*points));
		points[0] = nDPoint(imageIndex, 10, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0 ,48.0, 49.0, 50.0);
		*numOfFacturesExtracted = 1;
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

static bool kdTreeFactoryCreationAfterLoadTest() {
	SP_CONFIG_MSG configMsg;
	SPConfig config = spConfigCreate("./test_resources/tree_factory_test_config_after_load.txt", &configMsg);
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
	RUN_TEST(kdTreeFactoryCreationAfterLoadTest);
}
