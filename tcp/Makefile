include ../common.mk

MAIN_FILE = ./src/main.c
SECRET_KEY_FILE = ./src/isn_generation/secret_key/secret_key.c
COMMON_SRC_FILES = $(shell find . -name '*.c' ! -name '*.test.c' ! -path $(MAIN_FILE) ! -path $(SECRET_KEY_FILE))
HEADER_FILES = $(shell find . -name '*.h')
TEST_FILES = $(shell find . -name '*.test.c')
SOCKETS_BINARY = $(BIN_DIR)/sockets
TEST_BINARY = $(BIN_DIR)/test

.PHONY: unit-test e2e-test clean

$(SECRET_KEY_FILE):
	echo "// This is an generated file and should not be edited directly!" > $(SECRET_KEY_FILE)
	head -c 16 /dev/random | xxd -i -n secret_key >> $(SECRET_KEY_FILE)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(SOCKETS_BINARY): $(COMMON_SRC_FILES) $(HEADER_FILES) $(SECRET_KEY_FILE) $(MAIN_FILE) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(SOCKETS_BINARY) $(COMMON_SRC_FILES) $(SECRET_KEY_FILE) $(MAIN_FILE)

$(TEST_BINARY): $(COMMON_SRC_FILES) $(HEADER_FILES) $(SECRET_KEY_FILE) $(TEST_FILES) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TEST_BINARY) $(COMMON_SRC_FILES) $(SECRET_KEY_FILE) $(TEST_FILES)

unit-test: $(TEST_BINARY) $(SOCKETS_BINARY)
	MallocNanoZone=0 $(TEST_BINARY)

e2e-test:
	./scripts/test.sh && echo "tests passed :)"

clean:
	rm -rf bin $(SECRET_KEY_FILE)
