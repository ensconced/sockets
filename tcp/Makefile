include ../common.mk

MAIN_FILE = ./src/main.c
SECRET_KEY_FILE = ./src/isn_generation/secret_key/secret_key.c
COMMON_SRC_FILES = $(shell find . -name '*.c' ! -name '*.test.c' ! -path $(MAIN_FILE) ! -path $(SECRET_KEY_FILE))
HEADER_FILES = $(shell find . -name '*.h')
TEST_FILES = $(shell find . -name '*.test.c')
SERVER_BINARY = $(BIN_DIR)/serve
TEST_BINARY = $(BIN_DIR)/test

.PHONY: test serve clean

$(SECRET_KEY_FILE):
	echo "// This is an generated file and should not be edited directly!" > $(SECRET_KEY_FILE)
	head -c 16 /dev/random | xxd -i -n secret_key >> $(SECRET_KEY_FILE)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(SERVER_BINARY): $(COMMON_SRC_FILES) $(HEADER_FILES) $(SECRET_KEY_FILE) $(MAIN_FILE) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(SERVER_BINARY) $(COMMON_SRC_FILES) $(SECRET_KEY_FILE) $(MAIN_FILE)

$(TEST_BINARY): $(COMMON_SRC_FILES) $(HEADER_FILES) $(SECRET_KEY_FILE) $(TEST_FILES) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TEST_BINARY) $(COMMON_SRC_FILES) $(SECRET_KEY_FILE) $(TEST_FILES)

test: $(TEST_BINARY) $(SERVER_BINARY)
	MallocNanoZone=0 $(TEST_BINARY) && ./scripts/test.sh && echo "tests passed :)"

serve: $(SERVER_BINARY)
	# disable MallocNanoZone as a fix for this issue on macOS
	# https://stackoverflow.com/questions/69861144/get-an-error-as-a-out40780-0x1130af600-malloc-nano-zone-abandoned-due-to-in
	MallocNanoZone=0 $(SERVER_BINARY)

clean:
	rm -rf bin $(SECRET_KEY_FILE)
