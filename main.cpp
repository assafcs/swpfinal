/*
 * main.cpp

 *
 *  Created on: Jul 24, 2016
 *      Author: mataneilat
 */

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include "SPImageProc.h"
extern "C" {
#include "SPLogger.h"
#include "SPKDTree.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"
#include "sp_kd_tree_factory.h"
#include "sp_similar_images_search_api.h"
}

/**
 * Main SPCBIR implementation
 *
 * The SPCBIR basic functionality is to find similar images to a given query image, by searching in
 * pre-processed data-structure containing different images information (the kd-tree).
 *
 */

/*** Constants ***/

/** Different constants for log messages. */

#define NON_MINIMAL_GUI_RESULTS_TITLE_PREFIX "Best candidates for - "
#define NON_MINIMAL_GUI_RESULTS_TITLE_SUFFIX " - are:"

#define INVALID_COMMAND_LINE_TEXT "Invalid command line : use -c <config_filename>\n"

#define ENDING_QUERIES_STRING "<>"

#define QUERY_IMAGE_INPUT "Please enter an image path:\n"
#define EXIT_MESSAGE "Exiting...\n"

#define SP_LOGGER_CANNOT_OPEN_FILE_TEXT "The logger output file cannot be opened\n"

#define SP_IMAGE_PROC_CREATION_ERROR_MSG "Could not initialize SPImageProc instance."

#define TREE_CREATION_FATAL_ERROR_MSG "Could not initialize kd-tree properly"
#define TREE_CREATION_NON_FATAL_ERROR_MSG "KD-Tree was created, but some of the operations did not finish successfully. \n"
#define TREE_SUCCESSFULLY_CREATE_MSG "KD-Tree was successfully created"

#define QUERY_IMAGE_SEARCH_FAIL_MSG "Similar images search failed for path:"
#define SHOW_IMAGE_FAIL_MSG "Could not show image at path:"
#define QUERY_RESULT_COUNT_MSG "Image search complete, the number of results is:"

using namespace sp;

/**
 * Deallocates the given parameters and logger instance.
 *
 * @param config SPConfig instance to destroy.
 * @param searchTree SPKDTreeNode instance to destroy.
 * @param currentResultImagePath String to deallocate.
 * @param filename String to deallocate.
 * @param imageQueryPath String to deallocate.
 */
void freeAll(SPConfig config, SPKDTreeNode searchTree, char *currentResultImagePath, char *filename, char *imageQueryPath) {
	spConfigDestroy(config);
	spLoggerDestroy();
	spKDTreeDestroy(searchTree);
	free(currentResultImagePath);
	free(filename);
	free(imageQueryPath);
}

/**
 * Main Function of SPCBIR.
 * Creates a kd-tree by the configured parameters, and searches for similar images of user's query paths.
 *
 * In the arguments, one can configure a desired configuration file path by using -c flag as follows:
 * 		./SPCBIR -c myconfig.config
 * If not configured, the program will use a default value: spcbir.config
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 *
 * @return
 * 	The return code: 1 on failure, 0 on success.
 */
int main(int argc, char *argv[]) {
	char logMSG[LOGGER_MSG_LENGTH] = { '\0' };
	// init with nulls for destroy methods
	SPKDTreeNode searchTree = NULL;
	static ImageProc *ipPtr = NULL;
	int resultsCount;
	// Input validation and in case of no config, default config file setting
	char *currentResultImagePath = NULL, *imageQueryPath = NULL, *filename = (char *) malloc(LINE_MAX_SIZE * sizeof(char));

	if (filename == NULL) {
		printRErrorMsg(__FILE__, __LINE__, ALLOCATION_ERROR_MSG);
		return 1;
	}

	if (argc > 3 || argc == 2 || (argc == 3 && strcmp(argv[1], "-c") != 0)) {
		printf(INVALID_COMMAND_LINE_TEXT);
		free(filename);
		return 1;
	} else if (argc == 1) {
		strcpy(filename, DEFAULT_CONFIG_FILENAME);
	} else {
		strcpy(filename, argv[2]);
	}

	// Creating config file and exiting on failure.
	// Error messages handling by SPConfig.c
	SP_CONFIG_MSG resultMSG;
	SPConfig config = spConfigCreate(filename, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		free(filename);
		return 1;
	}

	// Creating logger
	SP_LOGGER_MSG loggerMSG = spLoggerCreate(spConfigGetLoggerFilename(config), spConfigGetLoggerLevel(config));
	if (loggerMSG != SP_LOGGER_SUCCESS){
		switch (loggerMSG) {
			case SP_LOGGER_CANNOT_OPEN_FILE:
				printf(SP_LOGGER_CANNOT_OPEN_FILE_TEXT);
				break;
			case SP_LOGGER_OUT_OF_MEMORY:
				printf(ALLOCATION_ERROR_MSG);
				break;
			default:
				break;
		}
		freeAll(config, searchTree, currentResultImagePath, filename, imageQueryPath);
		return 1;
	}

	try {
		static ImageProc ip = ImageProc(config);
		ipPtr = &ip;
	} catch (...) {
		printRErrorMsg(__FILE__, __LINE__, SP_IMAGE_PROC_CREATION_ERROR_MSG);
		freeAll(config, searchTree, currentResultImagePath, filename, imageQueryPath);
		return 1;
	}

	FeatureExractionFunction func = [] (const char *imagePath, int imageIndex, int *numOfFeaturesExtracted)->SPPoint* {
		try {
			return (*ipPtr).getImageFeatures(imagePath, imageIndex, numOfFeaturesExtracted);
		} catch (...) {
			*numOfFeaturesExtracted = 0;
			return NULL;
		}
	};

	SP_KD_TREE_CREATION_MSG treeCreationMsg;
	searchTree = spImagesKDTreeCreate(config, func, &treeCreationMsg);
	if (treeCreationMsg == SP_KD_TREE_CREATION_SUCCESS) {
		spLoggerPrintInfo(TREE_SUCCESSFULLY_CREATE_MSG);
	} else {
		if (treeCreationMsg != SP_KD_TREE_CREATION_NON_FATAL_ERROR) {
			sprintf(logMSG, "%s, %s %d", TREE_CREATION_FATAL_ERROR_MSG, RETURN_VALUE_MSG, treeCreationMsg);
			spLoggerPrintDebug(logMSG, __FILE__, __func__, __LINE__);
			printRErrorMsg(__FILE__, __LINE__, TREE_CREATION_FATAL_ERROR_MSG);
			freeAll(config, searchTree, currentResultImagePath, filename, imageQueryPath);
			return 1;
		} else {
			printf(TREE_CREATION_NON_FATAL_ERROR_MSG);
		}
	}

	imageQueryPath = (char *) malloc(LINE_MAX_SIZE * sizeof(char));
	currentResultImagePath = (char *) malloc (MAX_PATH_LENGTH * sizeof(char));

	if (imageQueryPath == NULL || currentResultImagePath == NULL) {
		printRErrorMsg(__FILE__, __LINE__, ALLOCATION_ERROR_MSG);
		freeAll(config, searchTree, currentResultImagePath, filename, imageQueryPath);
		return 1;
	}

	while (true) {

		printf(QUERY_IMAGE_INPUT);
		scanf("%s", imageQueryPath);

		if (strcmp(imageQueryPath, ENDING_QUERIES_STRING) == 0) {
			break;
		} else {

			SP_SIMILAR_IMAGES_SEARCH_API_MSG queryMsg;
			int *similarImages = spFindSimilarImagesIndices(config, imageQueryPath, searchTree, &resultsCount, func, &queryMsg);

			sprintf(logMSG, "%s %d", QUERY_RESULT_COUNT_MSG, resultsCount);
			spLoggerPrintInfo(logMSG);

			if (queryMsg != SP_SIMILAR_IMAGES_SEARCH_API_SUCCESS) {
				sprintf(logMSG, "%s %s %s %d", QUERY_IMAGE_SEARCH_FAIL_MSG, imageQueryPath, RETURN_VALUE_MSG, queryMsg);
				spLoggerPrintError(logMSG, __FILE__, __func__, __LINE__);
				printf("%s %s\n", QUERY_IMAGE_SEARCH_FAIL_MSG, imageQueryPath);
				continue;
			}

			if (!spConfigGetMinimalGuiPreference(config)) {
				printf("%s%s%s\n", NON_MINIMAL_GUI_RESULTS_TITLE_PREFIX, imageQueryPath, NON_MINIMAL_GUI_RESULTS_TITLE_SUFFIX);
			}

			for (int i = 0; i < resultsCount; i++) {
				spConfigGetImagePath(currentResultImagePath, config, similarImages[i]);

				if (spConfigGetMinimalGuiPreference(config)) {
					try {
						(*ipPtr).showImage(currentResultImagePath);
					} catch (...) {
						printf("%s %s\n", SHOW_IMAGE_FAIL_MSG, currentResultImagePath);
					}
				} else {
					printf("%s\n", currentResultImagePath);
				}
			}
			printf("\n");

		}
	}
	freeAll(config, searchTree, currentResultImagePath, filename, imageQueryPath);
	printf(EXIT_MESSAGE);
	return 0;
}


