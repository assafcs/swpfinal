/*
 * sp_similar_images_search_api.h
 *
 *  Created on: Sep 12, 2016
 *      Author: mataneilat
 */

#ifndef SP_SIMILAR_IMAGES_SEARCH_API_H_
#define SP_SIMILAR_IMAGES_SEARCH_API_H_

#include <stdlib.h>
#include "sp_constants.h"
#include "SPConfig.h"
#include "SPKDTree.h"

/**
 * Implementation of the similar images querying logic.
 *
 * The following functions are available:
 * 		spFindSimilarImagesIndices			- Finds the indices of the the most similar images,
 * 											  based on kd-tree nearest features search.
 */

/** Enumeration to inform result of API method calls. */
typedef enum sp_similar_images_search_api_msg_t {
	SP_SIMILAR_IMAGES_SEARCH_API_INVALID_ARGUMENT,
	SP_SIMILAR_IMAGES_SEARCH_API_CONFIG_ERROR,
	SP_SIMILAR_IMAGES_SEARCH_API_ALLOC_FAIL,
	SP_SIMILAR_IMAGES_SEARCH_API_FEATURES_EXTRACTION_ERROR,
	SP_SIMILAR_IMAGES_SEARCH_API_SUCCESS
} SP_SIMILAR_IMAGES_SEARCH_API_MSG;

/**
 * Finds the indices of the images most similar to a given image, by:
 * 	- Extracting the image's features.
 * 	- Finding the nearest features for each feature.
 * 	- Counting the most repeated images.
 *
 * The different parameters for the run (Nearest neighbors count, similar images count, etc.) are provided by the given configuration
 *
 * @param config The configuration used to provide the different parameters for the search.
 * @param queryImagePath The queried image, meaning the image that the result images should be similar to.
 * @param searchTree The kd-tree containing the different image's features to perform nearest nearest neighbor algorithm.
 * @param resultCount Place-holder for the amount of indices in the result
 * @param extractionFunc Function used to extract the image's features.
 * @param msg Place-holder for SP_SIMILAR_IMAGES_SEARCH_API_MSG to inform the process result:
 * 		SP_SIMILAR_IMAGES_SEARCH_API_INVALID_ARGUMENT 			- In case one of the given arguments is NULL.
 *		SP_SIMILAR_IMAGES_SEARCH_API_CONFIG_ERROR				- In case of configuration access error.
 *		SP_SIMILAR_IMAGES_SEARCH_API_ALLOC_FAIL					- In case of allocation failure.
 *		SP_SIMILAR_IMAGES_SEARCH_API_FEATURES_EXTRACTION_ERROR	- In case image's features extraction went wrong.
 *		SP_SIMILAR_IMAGES_SEARCH_API_SUCCESS					- In case search finished successfully.
 *
 * @return
 * 	NULL in case of a non-successful search.
 * 	Otherwise, returns the indices of the most similar images.
 *
 */
int *spFindSimilarImagesIndices(const SPConfig config, const char *queryImagePath,
		const SPKDTreeNode searchTree, int *resultsCount, FeatureExractionFunction extractionFunc,
		SP_SIMILAR_IMAGES_SEARCH_API_MSG *msg);


#endif /* SP_SIMILAR_IMAGES_SEARCH_API_H_ */
