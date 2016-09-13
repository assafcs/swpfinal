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

/**
 * The API functions to read/write features from a file.
 *
 * The following functions are available:
 * 		spFeaturesFileAPILoad 		- Loads a features array from the given file.
 * 		spFeaturesFileAPIWrite		- Writes a features array to a given file.
 */

/** Enumeration to inform result of API method calls. */
typedef enum sp_features_file_api_msg_t {
	SP_FEATURES_FILE_API_INVALID_ARGUMENT,
	SP_FEATURES_FILE_API_FEATURE_FILE_MISSING,
	SP_FEATURES_FILE_API_ALLOC_FAIL,
	SP_FEATURES_FILE_API_WRITE_ERROR ,
	SP_FEATURES_FILE_API_READ_ERROR,
	SP_FEATURES_FILE_API_SUCCESS
} SP_FEATURES_FILE_API_MSG;

/** The maximum base 10 string representation length of each point coordinate. */
static const int MAX_FEATURE_COORDINATE_STRING_LEN = 20;

/** The maximum base 10 string representation length of the number of features. */
static const int MAX_NUM_OF_FEATURES_STRING_LEN = 10;

/** The delimiter between features coordinates. */
static const char FEATURE_COORDINATES_DELIM = ' ';

/**
 * Reads features from the given file.
 *
 * @param filePath The path to the file containing the features data.
 * @param index The index of the image, to be put in each extracted point.
 * @param expectedFeatureDimension The expected dimension of the point.
 * @param numOfFeaturesLoaded Place-holder for the number of features loaded from the file.
 * @param msg Place-holder for the SP_FEATURES_FILE_API_MSG informing the process result:
 *		SP_FEATURES_FILE_API_INVALID_ARGUMENT 		- In case any pointer parameter is NULL,
 *													  or expected feature dimension is non-positive.
 *		SP_FEATURES_FILE_API_FEATURE_FILE_MISSING 	- In case the features file is missing
 *		SP_FEATURES_FILE_API_ALLOC_FAIL				- In case an allocation failure occurred.
 *		SP_FEATURES_FILE_API_READ_ERROR				- In case reading from the features file went wrong.
 *		SP_FEATURES_FILE_API_SUCCESS				- In case of successful load of features.
 *
 * @return
 *	NULL in case of a non-successful load.
 *	Otherwise, returns the features loaded from the given file.
 */
SPPoint *spFeaturesFileAPILoad(const char *filePath, int index, int expectedFeatureDimension, int *numOfFeaturesLoaded,
		SP_FEATURES_FILE_API_MSG *msg);

/**
 * Writes features to a given file.
 *
 * @param filePath The path to the file to write the features data to.
 * @param features The features to write to the file.
 * @param numOfFeatures The number of features in the features array.
 *
 * @return
 * 	 SP_FEATURES_FILE_API_MSG informing the method result status:
 * 	 	SP_FEATURES_FILE_API_INVALID_ARGUMENT		- In case filePath of features is NULL, or numOfFeatures is non-positive,
 * 	 												  or one of the following assumptions is somehow violated:
 * 	 												  	- numOfFeatures base 10 representation is smaller than MAX_NUM_OF_FEATURES_STRING_LEN digits.
 * 	 												  	- Each feature base 10 representation is smaller than MAX_FEATURE_COORDINATE_STRING_LEN digits.
 * 	 	SP_FEATURES_FILE_API_ALLOC_FAIL				- In case an allocation failure occurred.
 * 	 	SP_FEATURES_FILE_API_WRITE_ERROR			- In case writing to the features file went wrong.
 * 	 	SP_FEATURES_FILE_API_SUCCESS				- In case of successful features write.
 */
SP_FEATURES_FILE_API_MSG spFeaturesFileAPIWrite(const char *filePath, const SPPoint *features, int numOfFeatures);


#endif /* SP_FEATURES_FILE_API_H_ */
