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

typedef SPPoint *(*FeatureExractionFunction)(const char *, int, int *);

#define DEFAULT_CONFIG_FILENAME "spcbir.config"
#define LINE_MAX_SIZE 1024
#define ALLOCATION_ERROR_MSG "Memory allocation failed"
static const int MAX_PATH_LENGTH = 100;

#endif /* SP_CONSTANTS_H_ */
