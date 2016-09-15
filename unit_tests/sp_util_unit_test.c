/*
 * sp_util_unit_test.c
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../sp_util.h"
#include "unit_test_util.h"

#define ASSERT_STRING_EQUAL(first, second) ASSERT_SAME(strcmp(first, second), 0)

static bool spUtilSimpleSplitTest() {
	int numberOfStrings = 0;
	char **result = spUtilStrSplit("This is a string", ' ', &numberOfStrings);
	ASSERT_NOT_NULL(result);
	ASSERT_SAME(numberOfStrings, 4);

	ASSERT_STRING_EQUAL(result[0], "This");
	ASSERT_STRING_EQUAL(result[1], "is");
	ASSERT_STRING_EQUAL(result[2], "a");
	ASSERT_STRING_EQUAL(result[3], "string");

	spUtilFreeStringsArray(result, 4);
	return true;
}

static bool spUtilSplitWithEmptyEdgeStringsTest() {
	int numberOfStrings = 0;
	char **result = spUtilStrSplit("_This is_", '_', &numberOfStrings);
	ASSERT_NOT_NULL(result);
	ASSERT_SAME(numberOfStrings, 3);

	ASSERT_STRING_EQUAL(result[0], "");
	ASSERT_STRING_EQUAL(result[1], "This is");
	ASSERT_STRING_EQUAL(result[2], "");

	spUtilFreeStringsArray(result, 3);

	return true;
}

static bool spUtilSplitSingleDelimiterTest() {
	int numberOfStrings = 0;
	char **result = spUtilStrSplit("-", '-', &numberOfStrings);

	ASSERT_NOT_NULL(result);
	ASSERT_SAME(numberOfStrings, 2);

	ASSERT_STRING_EQUAL(result[0], "");
	ASSERT_STRING_EQUAL(result[1], "");

	spUtilFreeStringsArray(result, 2);

	return true;
}

static bool spUtilSplitNoDelimiterTest() {
	int numberOfStrings = 0;
	char **result = spUtilStrSplit("NN", ' ', &numberOfStrings);

	ASSERT_NOT_NULL(result);
	ASSERT_SAME(numberOfStrings, 1);

	ASSERT_STRING_EQUAL(result[0], "NN");

	spUtilFreeStringsArray(result, 1);

	return true;
}

static bool spUtilSplitConsecutiveDeilimitersTest() {
	int numberOfStrings = 0;
	char **result = spUtilStrSplit("NNM", 'N', &numberOfStrings);

	ASSERT_NOT_NULL(result);
	ASSERT_SAME(numberOfStrings, 3);

	ASSERT_STRING_EQUAL(result[0], "");
	ASSERT_STRING_EQUAL(result[1], "");
	ASSERT_STRING_EQUAL(result[2], "M");

	spUtilFreeStringsArray(result, 3);

	return true;
}

static bool spUtilSimpleJoinTest() {
	int numberOfStrings = 5;
	char *joinedString;
	char **strs = (char **) malloc(numberOfStrings * sizeof(char *));
	strs[0] = "1";
	strs[1] = "Two";
	strs[2] = "3";
	strs[3] = "Four";
	strs[4] = "5";

	joinedString = spUtilStrJoin((const char **)strs, numberOfStrings, '^');

	ASSERT_NOT_NULL(joinedString);
	ASSERT_STRING_EQUAL(joinedString, "1^Two^3^Four^5");

	free(strs);
	free(joinedString);
	return true;
}

static bool spUtilJoinSingleStringTest() {
	int numberOfStrings = 1;
	char *joinedString;
	char **strs = (char **) malloc(numberOfStrings * sizeof(char *));
	strs[0] = "1";

	joinedString = spUtilStrJoin((const char **)strs, numberOfStrings, '^');

	ASSERT_NOT_NULL(joinedString);
	ASSERT_STRING_EQUAL(joinedString, "1");

	free(strs);
	free(joinedString);
	return true;
}

int main() {
	printf("Running SPUtilTest.. \n");
	RUN_TEST(spUtilSimpleSplitTest);
	RUN_TEST(spUtilSplitWithEmptyEdgeStringsTest);
	RUN_TEST(spUtilSplitSingleDelimiterTest);
	RUN_TEST(spUtilSplitNoDelimiterTest);
	RUN_TEST(spUtilSplitConsecutiveDeilimitersTest);
	RUN_TEST(spUtilSimpleJoinTest);
	RUN_TEST(spUtilJoinSingleStringTest);
}
