/*
 * sp_constants.h
 *
 *  Created on: Sep 12, 2016
 *      Author: mataneilat
 */

#ifndef SP_CONSTANTS_H_
#define SP_CONSTANTS_H_

#include <stdlib.h>
#include "SPPoint.h"

/**
 * General constants to be used throughout.
 */

/**
 * Feature extract function declaration.
 * 	- First parameter is the image path.
 * 	- Second parameter is the image index.
 * 	- Third parameter is place-holder for the number of features extracted.
 */
typedef SPPoint *(*FeatureExractionFunction)(const char *, int, int *);

/** The default configuration file name.*/
#define DEFAULT_CONFIG_FILENAME "spcbir.config"

/** Maximum size for a single line to be used in different scenarios. */
#define LINE_MAX_SIZE 1024

/** Error message for allocation failure. */
#define ALLOCATION_ERROR_MSG "Memory allocation failed"

/** Maximum size for a path to be used in different scenarios. */
static const int MAX_PATH_LENGTH = 100;

#endif /* SP_CONSTANTS_H_ */
