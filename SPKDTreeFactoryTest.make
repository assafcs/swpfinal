CC = gcc
OBJS = sp_kd_tree_factory_unit_test.o common_test_util.o sp_kd_tree_factory.o sp_features_file_api.o sp_util.o SPKDTree.o SPKDArray.o SPPoint.o SPConfig.o SPParameterReader.o SPLogger.o
EXEC = sp_kd_tree_factory_unit_test
TESTS_DIR = ./unit_tests
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS) 
	$(CC) $(OBJS) -o $@
sp_kd_tree_factory_unit_test.o: $(TESTS_DIR)/sp_kd_tree_factory_unit_test.c $(TESTS_DIR)/unit_test_util.h SPPoint.h SPKDArray.h SPKDTree.h sp_kd_tree_factory.o
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c
common_test_util.o: $(TESTS_DIR)/common_test_util.c $(TESTS_DIR)/common_test_util.h
sp_util.o: sp_util.c sp_util.h
	$(CC) $(C_COMP_FLAG) -c $*.c
sp_features_file_api.o: sp_features_file_api.c sp_features_file_api.h
	$(CC) $(C_COMP_FLAG) -c $*.c
sp_kd_tree_factory.o: sp_kd_tree_factory.c sp_kd_tree_factory.h sp_features_file_api.h SPKDArray.h SPKDTree.h SPConfig.h sp_constants.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPKDTree.o: SPKDTree.c SPKDTree.h SPKDArray.h SPPoint.h
	$(CC) $(COMP_FLAG) -c $*.c
SPKDArray.o: SPKDArray.c SPKDArray.h SPPoint.h 
	$(CC) $(COMP_FLAG) -c $*.c
SPPoint.o: SPPoint.c SPPoint.h
	$(CC) $(COMP_FLAG) -c $*.c
SPParameterReader.o: SPParameterReader.c SPParameterReader.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPConfig.o: SPConfig.c SPConfig.h SPParameterReader.h sp_constants.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPLogger.o: SPLogger.c SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

clean: 
	rm -f $(OBJS) $(EXEC)
