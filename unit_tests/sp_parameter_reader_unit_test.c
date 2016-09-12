/*
 * sp_parameter_reader_test.c
 *
 *  Created on: Sep 11, 2016
 *      Author: mataneilat
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
#include <string.h>
#include "unit_test_util.h"
#include "../SPParameterReader.h"

static bool invalidParameterRead(SPParameterReader parameterReader, SP_PARAMETER_READ_MSG expectedMsg, bool isEOFExpected);
static bool properParametersRead(SPParameterReader parameterReader, const char *expectedKey,
		const char *expectedValue, bool isEOFExpected);

static bool spParameterReadTest() {
	SPParameterReader parameterReader = spParameterReaderInit(fopen("./test_resources/test_config.txt", "r"));

	ASSERT_NOT_NULL(parameterReader);
	ASSERT(invalidParameterRead(parameterReader, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(parameterReader, SP_PARAMETER_READ_EMPTY_LINE, false));
	ASSERT(invalidParameterRead(parameterReader, SP_PARAMETER_READ_COMMENT_LINE, false));
	ASSERT(invalidParameterRead(parameterReader, SP_PARAMETER_READ_COMMENT_LINE, false));
	ASSERT(invalidParameterRead(parameterReader, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(parameterReader, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(properParametersRead(parameterReader, "ABS", "CDS", false));
	ASSERT(properParametersRead(parameterReader, "M", "2", false));
	ASSERT(invalidParameterRead(parameterReader, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(parameterReader, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(parameterReader, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(properParametersRead(parameterReader, "ll", "123456789012345678901234567890", false));
	ASSERT(invalidParameterRead(parameterReader, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(parameterReader, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(parameterReader, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(parameterReader, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(properParametersRead(parameterReader, "d", "3", true));

	spParameterReaderDestroy(parameterReader);
	return true;
}

/*** Help Assert Methods ***/

static bool invalidParameterRead(SPParameterReader parameterReader, SP_PARAMETER_READ_MSG expectedMsg, bool isEOFExpected) {
	SP_PARAMETER_READ_MSG resultMsg;
	KeyToValue *result = spParameterReaderNext(parameterReader, &resultMsg);
	ASSERT_NULL(result);
	ASSERT_SAME(resultMsg, expectedMsg);
	ASSERT_SAME(!isEOFExpected, spParameterReaderHasNext(parameterReader));
	spParameterReaderKeyToValueDestroy(result, true);
	return true;
}

static bool properParametersRead(SPParameterReader parameterReader, const char *expectedKey,
		const char *expectedValue, bool isEOFExpected) {
	SP_PARAMETER_READ_MSG resultMsg;
	KeyToValue *result = spParameterReaderNext(parameterReader, &resultMsg);
	ASSERT_SAME(resultMsg, SP_PARAMETER_READ_SUCCESS);
	ASSERT_SAME(strcmp(result->key, expectedKey), 0);
	ASSERT_SAME(strcmp(result->value, expectedValue), 0);
	ASSERT_SAME(!isEOFExpected, spParameterReaderHasNext(parameterReader));
	spParameterReaderKeyToValueDestroy(result, true);
	return true;
}

int main() {
	printf("Running SPParameterReaderTest.. \n");
	RUN_TEST(spParameterReadTest);
}
