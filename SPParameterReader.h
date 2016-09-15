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

/**
 * Data structure which provides the ability to read entries of key and value from a given file stream.
 *
 * Format Specifications:
 * 		- Each key and value entry should be in a different line.
 * 		- Between the key and value, there should be a separating '='
 * 		- Comment line (that will be avoided) should start with '#'.
 * 		- Spaces between valid components will be avoided
 * For example:
 * 		key1 = value1
 * 		key2=   value2
 *
 * The following functions are available:
 *			spParameterReaderInit				- Initializes the parameter reader with a file stream.
 *			spParameterReaderDestroy			- Deallocates the given parameter reader instance.
 *			spParameterReaderNext				- Returns the next pair of key and value from the stream.
 *			spParameterReaderHasNext			- Returns whether the parameter reader has more to read.
 *			spParameterReaderKeyToValueDestroy	- Deallocates the KeyToVaue data structure.
 */

/** Data-structure thats represents a parameter: key and value. */
typedef struct sp_key_to_value {
	char *key;
	char *value;
} KeyToValue;

/** Enumeration to indicate the result of the next parameter operation. */
typedef enum sp_parameter_read_msg_t {
	SP_PARAMETER_READ_INVALID_ARGUMENT,
	SP_PARAMETER_READ_INVALID_FORMAT,
	SP_PARAMETER_READ_ALLOCATION_FAILED,
	SP_PARAMETER_READ_EMPTY_LINE,
	SP_PARAMETER_READ_COMMENT_LINE,
	SP_PARAMETER_READ_SUCCESS
} SP_PARAMETER_READ_MSG;


/** Type for defining the parameter reader. */
typedef struct sp_param_reader_t *SPParameterReader;

/**
 * Initializes the parameter reader with a given file stream.
 *
 * @param stream The file stream to read parameters from.
 *
 * @return
 * 		NULL on non-successful init.
 * 		Otherwise returns the initialized parameter reader instance.
 */
SPParameterReader spParameterReaderInit(FILE *stream);

/**
 * Deallocates the given parameter reader.
 *
 * @param parameterReader The parameter reader to deallocate.
 *
 */
void spParameterReaderDestroy(SPParameterReader parameterReader);

/**
 * Returns the next parameter read from the stream - represented by a KeyToVaue instance.
 *
 * @param parameterReader The parameter reader to use.
 * @param msg - Place-holder for a message to indicate the result:
 * 			SP_PARAMETER_READ_INVALID_ARGUMENT			- In case the given parameterReader is NULL.
 * 			SP_PARAMETER_READ_INVALID_FORMAT			- In case the current line is written in a wrong format.
 * 			SP_PARAMETER_READ_ALLOCATION_FAILED			- In case of an allocation failure.
 * 			SP_PARAMETER_READ_EMPTY_LINE				- In case the current line is empty.
 * 			SP_PARAMETER_READ_COMMENT_LINE				- In case the current line is a comment line.
 * 			SP_PARAMETER_READ_SUCCESS					- In case of successful read.
 *
 * @return
 * 	NULL on non0successful read.
 * 	Otherwise, return the read parameter represented by a KeyToVaue instance.
 */
KeyToValue *spParameterReaderNext(SPParameterReader parameterReader, SP_PARAMETER_READ_MSG *msg);

/**
 * Returns whether the parameter reader could read more parameters.
 *
 * @param parameterReader The parameter reader to query.
 *
 * @return
 *  False in case the given parameter reader is NULL,
 *  Otherwise, returns whether more parameters can be read.
 */
bool spParameterReaderHasNext(SPParameterReader parameterReader);

/**
 * Deallocates a KeyToVaue instance.
 * Has the ability to keep the value, in case it used as a string.
 *
 * @param keyToValue The KeyToValue instance to deallocate.
 * @param destroyValue Whether to destroy the value or not.
 *
 */
void spParameterReaderKeyToValueDestroy(KeyToValue *keyToValue, bool destroyValue);

#endif /* SPPARAMETERREADER_H_ */
