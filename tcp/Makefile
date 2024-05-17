include ../common.mk

MAIN_FILE = ./src/main.c
SECRET_KEY_FILE = ./src/isn_generation/secret_key.c
DEFINES_FILE = ./src/defines.h
COMMON_FILES = $(shell find . -name '*.c' ! -name '*.test.c' ! -path $(MAIN_FILE) ! -path $(SECRET_KEY_FILE))
TEST_FILES = $(shell find . -name '*.test.c')
SERVER_BINARY = $(BIN_DIR)/serve
TEST_BINARY = $(BIN_DIR)/test

LOCAL_PORT = 3001
REMOTE_PORT = 3000

.PHONY: test serve clean

$(SECRET_KEY_FILE):
	echo "// This is an generated file and should not be edited directly!" > $(SECRET_KEY_FILE)
	head -c 16 /dev/random | xxd -i -n secret_key >> $(SECRET_KEY_FILE)

$(DEFINES_FILE):
	echo "// This is an generated file and should not be edited directly!\n"\
	"#pragma once\n"\
	"#define LOCAL_PORT $(LOCAL_PORT)\n"\
	"#define REMOTE_PORT $(REMOTE_PORT)"\
	> $(DEFINES_FILE)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(SERVER_BINARY): $(COMMON_FILES) $(SECRET_KEY_FILE) $(MAIN_FILE) $(DEFINES_FILE) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(SERVER_BINARY) $(COMMON_FILES) $(SECRET_KEY_FILE) $(MAIN_FILE)

$(TEST_BINARY): $(COMMON_FILES) $(SECRET_KEY_FILE) $(TEST_FILES) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TEST_BINARY) $(COMMON_FILES) $(SECRET_KEY_FILE) $(TEST_FILES)

test: $(TEST_BINARY)
	MallocNanoZone=0 $<

serve: $(SERVER_BINARY)
	@./scripts/serve "$(SERVER_BINARY)" "$(LOCAL_PORT)" "$(REMOTE_PORT)"

clean:
	rm -rf bin $(DEFINES_FILE) $(SECRET_KEY_FILE)