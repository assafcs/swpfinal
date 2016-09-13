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
#include "sp_constants.h"

/**
 * Factory which supplies initialization methods of features kd-trees out of configured images.
 *
 * The following functions are available:
 *
 * 		spImagesKDTreeCreate 		- Creates a kd-tree for the configured images,
 * 									  by extraction or by loading previously extracted features.
 *
 */

/** Enumeration to inform the result of creation methods. */
typedef enum sp_kd_tree_creation_msg_t {
	SP_KD_TREE_CREATION_FEATURES_EXTRACTION_ERROR,
	SP_KD_TREE_CREATION_INVALID_ARGUMENT,
	SP_KD_TREE_CREATION_FEATURE_FILE_MISSING,
	SP_KD_TREE_CREATION_CONFIG_ERROR,
	SP_KD_TREE_CREATION_ALLOC_FAIL,
	SP_KD_TREE_CREATION_WRITE_ERROR,
	SP_KD_TREE_CREATION_LOAD_ERROR,
	SP_KD_TREE_CREATION_SUCCESS
} SP_KD_TREE_CREATION_MSG;

/**
 * Creates a features kd-tree for the configured images, by extraction or by loading previously extracted features.
 *
 * If extraction mode is configured, the method will create the kd-tree and will also write the extracted images features to .feat
 * corresponding files, using the sp_features_file_api methods, which can be loaded by re-running with non-extraction mode.
 *
 * If non-extraction mode is configured, the method will create the kd-tree by loading the images features from the previously written .feats files,
 * using the sp_features_file_api methods.
 *
 * @param config The configuration to use in order to create the kd-tree.
 * @param featureExtractionFunction a function used for extracting images features if needed.
 * @param msg The SP_KD_TREE_CREATION_MSG informing the result of the creation:
 * 		SP_KD_TREE_CREATION_FEATURES_EXTRACTION_ERROR		- In case features were not extracted properly.
 * 		SP_KD_TREE_CREATION_INVALID_ARGUMENT				- In case one of the given parameters is NULL.
 * 		SP_KD_TREE_CREATION_FEATURE_FILE_MISSING			- In case non-extraction mode was set, but feature file was missing.
 * 		SP_KD_TREE_CREATION_CONFIG_ERROR					- In case of a configuration access error.
 * 		SP_KD_TREE_CREATION_ALLOC_FAIL						- In case of allocation failure.
 * 		SP_KD_TREE_CREATION_WRITE_ERROR						- In case features write to file system went wrong (in extraction mode).
 * 		SP_KD_TREE_CREATION_LOAD_ERROR						- In case features load from file system went wrong (in non-extraction mode).
 * 		SP_KD_TREE_CREATION_SUCCESS							- In case kd-tree was created successfully.
 *
 * @return
 * 	NULL in case of a non-successful creation.
 * 	Otherwise, returns the created features kd-tree for the configured images.
 */
SPKDTreeNode spImagesKDTreeCreate(const SPConfig config,
		FeatureExractionFunction featureExtractionFunction,
		SP_KD_TREE_CREATION_MSG *msg);

#endif /* SP_KD_TREE_FACTORY_H_ */
