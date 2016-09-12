/*
 * sp_features_file_api.h
 *
 *  Created on: Sep 11, 2016
 *      Author: mataneilat
 */

#ifndef SP_FEATURES_FILE_API_H_
#define SP_FEATURES_FILE_API_H_

#include "SPPoint.h"
#include "SPConfig.h"

typedef enum sp_features_file_api_msg_t {
	SP_FEATURES_FILE_API_CONFIGUATION_ERROR = 0,
	SP_FEATURES_FILE_API_FEATURES_EXTRACTION_ERROR = 1,
	SP_FEATURES_FILE_API_INVALID_ARGUMENT = 2,
	SP_FEATURES_FILE_API_FEATURE_FILE_MISSING = 3,
	SP_FEATURES_FILE_API_CONFIG_ERROR = 4,
	SP_FEATURES_FILE_API_ALLOC_FAIL = 5,
	SP_FEATURES_FILE_API_WRITE_ERROR = 6,
	SP_FEATURES_FILE_API_LOAD_ERROR = 7,
	SP_FEATURES_FILE_API_SUCCESS = 8
} SP_FEATURES_FILE_API_MSG;

SPPoint *loadFeatures(const char *filePath, int index, int expectedFeatureDimension, int *numOfFeaturesLoaded,
		SP_FEATURES_FILE_API_MSG *msg);

SP_FEATURES_FILE_API_MSG writeFeatures(char *filePath, SPPoint *features, int numOfFeatures);


#endif /* SP_FEATURES_FILE_API_H_ */
