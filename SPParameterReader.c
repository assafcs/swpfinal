/*
 * sp_param_read_utils.c
 *
 *  Created on: Sep 11, 2016
 *      Author: mataneilat
 */

#include "SPParameterReader.h"
#include <ctype.h>
#include <string.h>

/*** Type declarations ***/

/** Structure containing the kdArray data. */
struct sp_param_reader_t {
	FILE *stream;
	bool hasNext;
};

void addCharacterToWord(char c, char** word, int* wordSize, int* wordCapacity) {
	if ((*wordSize) >= (*wordCapacity)) {
		(*wordCapacity) *= 2;
		(*word) = (char*) realloc((*word), (*wordCapacity) * sizeof(char));
	}
	(*word)[*wordSize] = c;
	(*wordSize)++;
}

SPParameterReader spParameterReaderInit(FILE *stream) {
	if (stream == NULL) {
		return NULL;
	}
	SPParameterReader parameterReader = (SPParameterReader) malloc(sizeof(*parameterReader));
	parameterReader->stream = stream;
	parameterReader->hasNext = !feof(stream);

	return parameterReader;
}

void spParameterReaderDestroy(SPParameterReader parameterReader) {
	if (parameterReader == NULL) {
		return;
	}
	if (parameterReader->stream != NULL) {
		fclose(parameterReader->stream);
	}
	free(parameterReader);
}

KeyToValue *spParameterReaderNext(SPParameterReader parameterReader, SP_PARAMETER_READ_MSG *msg) {
	if (parameterReader == NULL) {
		*msg = SP_PARAMETER_READ_INVALID_ARGUMENT;
		return NULL;
	}

	KeyToValue *readParameter = NULL;
	char ch;
	int keyCapacity = 16, valueCapacity = 16, keySize = 0, valueSize = 0;
	int *currentCapacity = &keyCapacity;
	int *currentSize = &keySize;
	bool invalid = false, lineBreak = false, commentLine = false;
	char *key, *value, *outputKey, *outputValue;
	char **current;

	key = (char *) malloc(sizeof(char) * keyCapacity);
	if (key == NULL) {
		*msg = SP_PARAMETER_READ_ALLOCATION_FAILED;
		return NULL;
	}

	value = (char *) malloc(sizeof(char) * valueCapacity);
	if (key == NULL) {
		free(key);
		*msg = SP_PARAMETER_READ_ALLOCATION_FAILED;
		return NULL;
	}

	current = &key;

	while ((ch = getc(parameterReader->stream)) != EOF) {
		if (ch == '\n') {
			if (current == &value && valueSize > 0) {
				addCharacterToWord('\0', current, currentSize, currentCapacity);
				current = NULL;
				currentSize = currentCapacity = NULL;
			}
			lineBreak = true;
			break;
		}

		if (invalid || commentLine) {
			// Wait for new line
			continue;
		}

		if (isspace(ch)) {
			if ((current == &key && keySize > 0) || (current == &value && valueSize > 0)) {
				addCharacterToWord('\0', current, currentSize, currentCapacity);
				current = NULL;
				currentSize = currentCapacity = NULL;
			}
			continue;
		}

		if (ch == '=') {
			if (current == &key && keySize > 0) {
				addCharacterToWord('\0', current, currentSize, currentCapacity);
				current = NULL;
				currentSize = currentCapacity = NULL;
			}
			if (current == NULL && keySize > 0 && valueSize == 0) {
				current = &value;
				currentSize = &valueSize;
				currentCapacity = &valueCapacity;
			} else {
				invalid = true;
				continue;
			}
		} else {
			if (current == NULL) {
				invalid = true;
				continue;
			} else if (ch == '#' && current == &key && keySize == 0) {
				commentLine = true;
				continue;
			}
			addCharacterToWord(ch, current, currentSize, currentCapacity);
		}
	}

	if (!lineBreak) {
		// In case the loop exited because of EOF - complete the value if needed
		if (current == &value && valueSize > 0) {
			addCharacterToWord('\0', current, currentSize, currentCapacity);
			current = NULL;
			currentSize = currentCapacity = NULL;
		}
		// Informs the the stream reached EOF
		parameterReader->hasNext = false;
	}

	*msg = SP_PARAMETER_READ_INVALID_FORMAT;

	if (!invalid) {
		if (commentLine) {
			*msg = SP_PARAMETER_READ_COMMENT_LINE;
		} else if (keySize == 0 && valueSize == 0) {
			*msg = SP_PARAMETER_READ_EMPTY_LINE;
		} else if (current == NULL && keySize > 0 && valueSize > 0) {
			// Success reading key and value
			outputKey = (char *) malloc(keySize * sizeof(char));
			outputValue = (char *) malloc(valueSize * sizeof(char));
			readParameter = (KeyToValue *) malloc(sizeof(KeyToValue));
			if (outputKey == NULL || outputValue == NULL || readParameter == NULL) {
				free(outputKey);
				free(outputValue);
				free(readParameter);
				readParameter = NULL;
				*msg = SP_PARAMETER_READ_ALLOCATION_FAILED;
			} else {
				strcpy(outputKey, key);
				strcpy(outputValue, value);

				readParameter->key = outputKey;
				readParameter->value = outputValue;

				*msg = SP_PARAMETER_READ_SUCCESS;
			}
		}
	}

	free(key);
	free(value);

	return readParameter;
}

bool spParameterReaderHasNext(SPParameterReader parameterReader) {
	if (parameterReader == NULL) {
		return false;
	}
	return parameterReader->hasNext && !feof(parameterReader->stream);
}

