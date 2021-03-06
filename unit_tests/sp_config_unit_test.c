/*
 * sp_config_unit_test.c
 *
 *  Created on: Jul 20, 2016
 *      Author: mataneilat
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
#include <string.h>
#include "unit_test_util.h"
#include "../SPConfig.h"

static bool configCreateError(const char *filename, SP_CONFIG_MSG expectedMsg);

static bool spConfigNullFilenameTest() {
	ASSERT(configCreateError(NULL, SP_CONFIG_INVALID_ARGUMENT));
	return true;
}

static bool spConfigMissingFileTest() {
	ASSERT(configCreateError("missing", SP_CONFIG_CANNOT_OPEN_FILE));
	return true;
}

static bool spConfigMissingConfigFileTest() {
	ASSERT(configCreateError("./test_resources/test_config_missing.txt", SP_CONFIG_MISSING_SUFFIX));
	return true;
}

static bool spConfigProperConfigFileTest() {

	SP_CONFIG_MSG resultMsg;
	SPConfig config = spConfigCreate("./test_resources/test_config_1.txt", &resultMsg);

	ASSERT_NOT_NULL(config);
	ASSERT_SAME(resultMsg, SP_CONFIG_SUCCESS);

	char *imagesDirectory = spConfigImagesDirectory(config, &resultMsg);
	ASSERT_NOT_NULL(imagesDirectory);
	ASSERT_SAME(strcmp(imagesDirectory, "/tmp/testDirectory/"), 0);

	char *imagesPrefix = spConfigImagesPrefix(config, &resultMsg);
	ASSERT_NOT_NULL(imagesPrefix);
	ASSERT_SAME(strcmp(imagesPrefix, "sp"), 0);

	spConfigDestroy(config);
	return true;

}

static bool spConfigPCAPathTest() {
	SP_CONFIG_MSG resultMsg;
	SPConfig config = spConfigCreate("./test_resources/test_config_1.txt", &resultMsg);

	ASSERT_NOT_NULL(config);
	ASSERT_SAME(resultMsg, SP_CONFIG_SUCCESS);

	char *pcaPath = (char *) malloc(50 * sizeof(char));

	ASSERT_SAME(spConfigGetPCAPath(NULL, NULL), SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_SAME(spConfigGetPCAPath(NULL, config), SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_SAME(spConfigGetPCAPath(pcaPath, NULL), SP_CONFIG_INVALID_ARGUMENT);

	ASSERT_SAME(spConfigGetPCAPath(pcaPath, config), SP_CONFIG_SUCCESS);

	ASSERT_SAME(strcmp("/tmp/testDirectory/pca.yml", pcaPath), 0);

	free(pcaPath);
	spConfigDestroy(config);
	return true;
}

static bool spConfigImagesPathTest() {
	SP_CONFIG_MSG resultMsg;
	SPConfig config = spConfigCreate("./test_resources/test_config_1.txt", &resultMsg);

	ASSERT_NOT_NULL(config);
	ASSERT_SAME(resultMsg, SP_CONFIG_SUCCESS);

	char *imagePath = (char *) malloc(50 * sizeof(char));
	ASSERT_SAME(spConfigGetImagePath(NULL, NULL, 0), SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_SAME(spConfigGetImagePath(NULL, config, 0), SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_SAME(spConfigGetImagePath(imagePath, NULL, 0), SP_CONFIG_INVALID_ARGUMENT);

	ASSERT_SAME(spConfigGetImagePath(imagePath, config, 14), SP_CONFIG_INDEX_OUT_OF_RANGE);

	ASSERT_SAME(spConfigGetImagePath(imagePath, config, 3), SP_CONFIG_SUCCESS);

	ASSERT_SAME(strcmp("/tmp/testDirectory/sp3.img", imagePath), 0);

	free(imagePath);
	spConfigDestroy(config);
	return true;

}

/*** Help Assert Methods ***/

static bool configCreateError(const char *filename, SP_CONFIG_MSG expectedMsg) {
	SP_CONFIG_MSG returnMsg;
	SPConfig config = spConfigCreate(filename, &returnMsg);
	ASSERT_NULL(config);
	ASSERT_SAME(returnMsg, expectedMsg);
	return true;
}

int main() {
	printf("Running SPConfigTest.. \n");
	RUN_TEST(spConfigNullFilenameTest);
	RUN_TEST(spConfigMissingFileTest);
	RUN_TEST(spConfigProperConfigFileTest);
	RUN_TEST(spConfigMissingConfigFileTest);
	RUN_TEST(spConfigPCAPathTest);
	RUN_TEST(spConfigImagesPathTest);
}
