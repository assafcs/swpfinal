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

#define NON_MINIMAL_GUI_RESULTS_TITLE_PREFIX "Best candidates for - "
#define NON_MINIMAL_GUI_RESULTS_TITLE_SUFFIX " - are:\n"

#define INVALID_COMMAND_LINE_TEXT "Invalid command line : use -c <config_filename>\n"

#define ENDING_QUERIES_STRING "<>"

#define QUERY_IMAGE_INPUT "Please enter an image path:\n"
#define EXIT_MESSAGE "Exiting...\n"

#define SP_LOGGER_CANNOT_OPEN_FILE_TEXT "The logger output file cannot be opened\n"

using namespace sp;

void freeAll(SPConfig config, SPKDTreeNode searchTree, char *currentResultImagePath);

int main(int argc, char *argv[]) {

	// init with nulls for destroy methods
	SPKDTreeNode searchTree = NULL;
	char *currentResultImagePath = NULL;

	// Input validation and in case of no config, default config file setting
	char filename[LINE_MAX_SIZE];
	if (argc > 3 || argc == 2 || (argc == 3 && strcmp(argv[1], "-c") != 0)) {
		printf(INVALID_COMMAND_LINE_TEXT);
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
	if (resultMSG != SP_CONFIG_SUCCESS){
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
		}
		freeAll(config, searchTree, currentResultImagePath);
		return 1;
	}

	static ImageProc ip = ImageProc(config);
	FeatureExractionFunction func = [] (const char *imagePath, int imageIndex, int *numOfFeaturesExtracted)->SPPoint* {
		return ip.getImageFeatures(imagePath, imageIndex, numOfFeaturesExtracted);
	};

	SP_KD_TREE_CREATION_MSG treeCreationMsg;
	searchTree = spImagesKDTreeCreate(config, func, &treeCreationMsg);

	if (treeCreationMsg != SP_KD_TREE_CREATION_SUCCESS) {
		freeAll(config, searchTree, currentResultImagePath);
		//printf("Something went wrong with tree build"); // TODO: remove this print
		return 1;
	}


	char imageQueryPath[LINE_MAX_SIZE];
	int resultsCount;
	char *currentResultImagePath = (char *) malloc (MAX_PATH_LENGTH * sizeof(char));;
	while (true){
		printf(QUERY_IMAGE_INPUT);
		scanf("%s", imageQueryPath);
		if (strcmp(imageQueryPath, ENDING_QUERIES_STRING) == 0) {
			break;
		} else {
			if (!spConfigGetMinimalGuiPreference(config)){
				printf("%s%s%s", NON_MINIMAL_GUI_RESULTS_TITLE_PREFIX, imageQueryPath, NON_MINIMAL_GUI_RESULTS_TITLE_SUFFIX);
			}
			int *similarImages = spFindSimilarImagesIndices(config, imageQueryPath, searchTree, &resultsCount, func);
			//printf("Results count: %d \n", resultsCount);
			for (int i = 0; i < resultsCount; i++) {
				spConfigGetImagePath(currentResultImagePath, config, similarImages[i]);

				printf("%d ", similarImages[i]);
				if (spConfigGetMinimalGuiPreference(config)){
					ip.showImage(currentResultImagePath);
				} else {
					printf("%s%s", currentResultImagePath, "\n");
				}
			}
			printf("\n");

		}
	}
	freeAll(config, searchTree, currentResultImagePath);
	printf(EXIT_MESSAGE);
	return 0;
}

void freeAll(SPConfig config, SPKDTreeNode searchTree, char *currentResultImagePath){
	spConfigDestroy(config);
	spLoggerDestroy();
	spKDTreeDestroy(searchTree);
	free(currentResultImagePath);
}


