/*
 * sp_util.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#ifndef SP_UTIL_
#define SP_UTIL_

#include <stdlib.h>

/**
 * General utilities class to be used throughout the system.
 *
 * The following functions are given:
 *
 * 		spUtilStrSplit			- Splits a given string with respect to the given delimiter.
 * 		spUtilStrJoin			- Joins the given strings array using the given delimiter.
 *		spUtilFreeStringsArray	- Deallocates the given array of strings.
 */

/*** Constants ***/

/** The maximum size of each token between delimiters - used by the split method. */
static const int TOKEN_MAX_LEN = 100;


/**
 * Splits the given string with respect to the given delimiter.
 *
 * NOTICE that the maximum size of each token is set in a constant.
 *
 * @param str The string to split
 * @param delim The delimiter to split the string by.
 * @param stringsCount Output placeholder for the number of strings returned.
 *
 * @return
 * 	NULL if the given string is NULL, empty, or an allocation failure occurred.
 * 	Otherwise, returns the split strings.
 */
char **spUtilStrSplit(const char* str, const char delim, int *stringsCount);

/**
 * Joins the given strings array using the delimiter character between them.
 *
 * @param strs The strings array to join
 * @param stringsCount The strings array length
 * @param delim The delimiter to use a separator between every two consecutive strings.
 *
 * @return
 * 	NULL if the given strings array is NULL, or an allocation failure occurred.
 * 	Otherwise, returns the joined string, meaning a string which is the concatenation of the strings in the array,
 * 	using the given delimiter as a separator.
 */
char *spUtilStrJoin(const char **strs, int stringsCount, const char delim);

/**
 * Deallocates a given strings array.
 *
 * @param string The strings array to deallocate.
 * @param count The strings array size.
 *
 */
void spUtilFreeStringsArray(char **strings, int count);

#endif /* SP_UTIL_ */
