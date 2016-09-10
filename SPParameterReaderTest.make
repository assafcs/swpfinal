CC = gcc
OBJS = sp_parameter_reader_unit_test.o SPParameterReader.o
EXEC = sp_parameter_reader_unit_test
TESTS_DIR = ./unit_tests
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS) 
	$(CC) $(OBJS) -o $@
sp_parameter_reader_unit_test.o: $(TESTS_DIR)/sp_parameter_reader_unit_test.c $(TESTS_DIR)/unit_test_util.h
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c
SPParameterReader.o: SPParameterReader.c SPParameterReader.h
	$(CC) $(COMP_FLAG) -c $*.c
clean: 
	rm -f $(OBJS) $(EXEC)
