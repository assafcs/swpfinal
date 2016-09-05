/*
 * main_aux.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

extern "C" {
#include "SPPoint.h"
#include "SPConfig.h"
#include "SPKDTree.h"
}

#include "SPImageProc.h"

typedef enum sp_search_tree_creation_msg_t {
	SP_SEARCH_TREE_CREATION_FEATURES_CONFIGUATION_ERROR,
	SP_SEARCH_TREE_CREATION_FEATURES_EXTRACTION_ERROR,
	SP_SEARCH_TREE_CREATION_INVALID_ARGUMENT,
	SP_SEARCH_TREE_CREATION_FEATURE_FILE_MISSING,
	SP_SEARCH_TREE_CREATION_CONFIG_ERROR,
	SP_SEARCH_TREE_CREATION_ALLOC_FAIL,
	SP_SEARCH_TREE_CREATION_WRITE_ERROR,
	SP_SEARCH_TREE_CREATION_LOAD_ERROR,
	SP_SEARCH_TREE_CREATION_SUCCESS
} SP_SEARCH_TREE_CREATION_MSG;

using namespace sp;

SPKDTreeNode createImagesSearchTree(const SPConfig config, SP_SEARCH_TREE_CREATION_MSG *msg);

int *findSimilarImagesIndices(const SPConfig config, const char *queryImagePath, const SPKDTreeNode searchTree, int *resultsCount, ImageProc ip);


#endif /* MAIN_AUX_H_ */
