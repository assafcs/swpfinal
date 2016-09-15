CC = gcc
CPP = g++
#put your object files here
OBJS = sp_util.o sp_algorithms.o SPBPriorityQueue.o SPList.o SPListElement.o SPKDArray.o SPKDTree.o \
main.o SPImageProc.o SPPoint.o SPConfig.o SPParameterReader.o SPLogger.o sp_features_file_api.o sp_kd_tree_factory.o sp_similar_images_search_api.o
#The executabel filename
EXEC = SPCBIR
INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include
LIBPATH=/usr/local/lib/opencv-3.1.0/lib
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core

CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -L$(LIBPATH) $(LIBS) -o $@
main.o: main.cpp sp_kd_tree_factory.h sp_similar_images_search_api.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
SPImageProc.o: SPImageProc.cpp SPImageProc.h SPConfig.h SPPoint.h SPLogger.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
sp_similar_images_search_api.o: sp_similar_images_search_api.c sp_similar_images_search_api.h SPKDArray.h SPKDTree.h SPConfig.h SPPoint.h SPLogger.h sp_util.h sp_algorithms.h sp_constants.h
	$(CC) $(C_COMP_FLAG) -c $*.c
sp_util.o: sp_util.c sp_util.h
	$(CC) $(C_COMP_FLAG) -c $*.c
sp_algorithms.o: sp_algorithms.c sp_algorithms.h SPBPriorityQueue.h SPKDTree.h SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c
sp_features_file_api.o: sp_features_file_api.c sp_features_file_api.h sp_constants.h
	$(CC) $(C_COMP_FLAG) -c $*.c
sp_kd_tree_factory.o: sp_kd_tree_factory.c sp_kd_tree_factory.h sp_features_file_api.h SPKDArray.h SPKDTree.h SPConfig.h sp_constants.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPKDArray.o: SPKDArray.c SPKDArray.h SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPKDTree.o: SPKDTree.c SPKDTree.h SPKDArray.h SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPPoint.o: SPPoint.c SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPParameterReader.o: SPParameterReader.c SPParameterReader.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPConfig.o: SPConfig.c SPConfig.h SPParameterReader.h sp_constants.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPLogger.o: SPLogger.c SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h SPList.h SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPList.o: SPList.c SPList.h SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPListElement.o: SPListElement.c SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
