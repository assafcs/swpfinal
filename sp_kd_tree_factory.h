/*
 * sp_kd_tree_factory.h
 *
 *  Created on: Sep 11, 2016
 *      Author: mataneilat
 */

#ifndef SP_KD_TREE_FACTORY_H_
#define SP_KD_TREE_FACTORY_H_

#include <stdlib.h>
#include "SPKDTree.h"
#include "SPConfig.h"

typedef SPPoint *(*FeatureExractionFunction)(char *, int, int *);

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

SPKDTreeNode spImagesSearchTreeCreate(const SPConfig config,
		FeatureExractionFunction featureExtractionFunction,
		SP_SEARCH_TREE_CREATION_MSG *msg);

#endif /* SP_KD_TREE_FACTORY_H_ */
