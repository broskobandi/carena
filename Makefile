# Project
PROJECT := carena
CC := gcc
CFLAGS = -Wall -Wextra -Werror -Wconversion -Wunused-result
CPPFLAGS = -Iinclude $(EXTRA_CPPFLAGS)

# Dirs
BUILD_DIR := build
SRC_DIR := src
TEST_DIR := test
INC_DIR := include
OBJ_DIR := $(BUILD_DIR)/obj
LIB_INSTALL_DIR := /usr/local/lib
INC_INSTALL_DIR := /usr/local/include
DOC_DIR := doc
EXAMPLE_DIR := example

# Files
SRC := $(wildcard $(SRC_DIR)/*.c)
INC_PRIV := $(wildcard $(SRC_DIR)/*.h)
INC := $(INC_DIR)/$(PROJECT).h
LIB_A := $(BUILD_DIR)/lib$(PROJECT).a
LIB_SO := $(BUILD_DIR)/lib$(PROJECT).so
TEST_EXE := $(BUILD_DIR)/test
TEST_MAIN := $(TEST_DIR)/test.c
EXAMPLE_EXE := $(BUILD_DIR)/example
EXAMPLE_MAIN := $(EXAMPLE_DIR)/example.c
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Rules
.PHONY: all test example clean install uninstall doc tags

all: CFLAGS += -O3 -march=native -flto
all: CPPFLAGS += -DNDEBUG
all: $(LIB_A) $(LIB_SO)

test: CC := clang
test: CPPFLAGS += -Isrc -DTEST
test: LDFLAGS += -lctest
test: $(TEST_EXE)
	./$<

example: CC := clang
example: LDFLAGS = -lcarena 
example: $(EXAMPLE_EXE)

clean:
	rm -rf $(BUILD_DIR) $(DOC_DIR) compile_commands.json

install:
	cp $(LIB_A) $(LIB_INSTALL_DIR)
	cp $(LIB_SO) $(LIB_INSTALL_DIR)
	cp $(INC) $(INC_INSTALL_DIR)
	ldconfig

uninstall:
	rm $(addprefix $(LIB_INSTALL_DIR)/, $(notdir $(LIB_A)))
	rm $(addprefix $(LIB_INSTALL_DIR)/, $(notdir $(LIB_SO)))
	rm $(addprefix $(INC_INSTALL_DIR)/, $(notdir $(INC)))

doc:
	doxygen

tags:
	ctags -R --c-kinds=+p+d /usr/include /usr/local/include .

$(LIB_A): $(OBJ) | $(BUILD_DIR)
	ar rcs $@ $^

$(LIB_SO): $(OBJ) | $(BUILD_DIR)
	$(CC) -shared $(CFLAGS) $(CPPFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_PRIV) $(INC) | $(OBJ_DIR)
	$(CC) -c -fPIC $(CFLAGS) $(CPPFLAGS) $< -o $@

$(TEST_EXE): $(TEST_MAIN) $(OBJ) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

$(EXAMPLE_EXE): $(EXAMPLE_MAIN) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(EXTRA_CPPFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@
