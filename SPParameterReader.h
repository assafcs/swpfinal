/*
 * sp_param_read_utils.h
 *
 *  Created on: Sep 11, 2016
 *      Author: mataneilat
 */

#ifndef SPPARAMETERREADER_H_
#define SPPARAMETERREADER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct sp_key_to_value {
	char *key;
	char *value;
} KeyToValue;

typedef enum sp_parameter_read_msg_t {
	SP_PARAMETER_READ_INVALID_ARGUMENT,
	SP_PARAMETER_READ_INVALID_FORMAT,
	SP_PARAMETER_READ_ALLOCATION_FAILED,
	SP_PARAMETER_READ_EMPTY_LINE,
	SP_PARAMETER_READ_COMMENT_LINE,
	SP_PARAMETER_READ_SUCCESS
} SP_PARAMETER_READ_MSG;


/** Type for defining the kd-tree. */
typedef struct sp_param_reader_t *SPParameterReader;

SPParameterReader spParameterReaderInit(FILE *stream);

void spParameterReaderDestroy(SPParameterReader parameterReader);

KeyToValue *spParameterReaderNext(SPParameterReader parameterReader, SP_PARAMETER_READ_MSG *msg);

bool spParameterReaderHasNext(SPParameterReader parameterReader);

void spParameterReaderKeyToValueDestroy(KeyToValue *keyToValue, bool destroyValue);

#endif /* SPPARAMETERREADER_H_ */
