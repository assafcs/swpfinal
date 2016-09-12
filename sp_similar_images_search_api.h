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

int *spFindSimilarImagesIndices(const SPConfig config, const char *queryImagePath,
		const SPKDTreeNode searchTree, int *resultsCount, FeatureExractionFunction extractionFunc);


#endif /* SP_SIMILAR_IMAGES_SEARCH_API_H_ */
