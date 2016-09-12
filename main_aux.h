/*
 * main_aux.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

extern "C" {
#include "SPPoint.h"
#include "SPConfig.h"
#include "SPKDTree.h"
}

#include "SPImageProc.h"


using namespace sp;

int *findSimilarImagesIndices(const SPConfig config, const char *queryImagePath, const SPKDTreeNode searchTree, int *resultsCount, ImageProc ip);


#endif /* MAIN_AUX_H_ */
