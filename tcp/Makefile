include ../common.mk

# TODO - do I need to use sort/uniq here to avoid pulling in secret_key twice?
SOURCE_FILES = $(shell find . -name '*.c' ! -name '*.test.c' ! -name main.c ! -name secret_key.c)
TEST_FILES = $(shell find . -name '*.test.c')
MAIN_FILE = src/main.c
SECRET_KEY_FILE = src/isn_generation/secret_key.c
DEFINES_FILE = src/defines.h

LOCAL_PORT = 3001
REMOTE_PORT = 3000

$(SECRET_KEY_FILE):
	echo "// This is an generated file and should not be edited directly!" > $(SECRET_KEY_FILE)
	head -c 16 /dev/random | xxd -i -n secret_key >> $(SECRET_KEY_FILE)

$(DEFINES_FILE):
	echo "// This is an generated file and should not be edited directly!"\n#pragma once\n#define LOCAL_PORT $(LOCAL_PORT)\n#define REMOTE_PORT $(REMOTE_PORT)" > $(DEFINES_FILE)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/serve: $(SOURCE_FILES) $(SECRET_KEY_FILE) $(MAIN_FILE) $(DEFINES_FILE) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SOURCE_FILES) $(SECRET_KEY_FILE) $(MAIN_FILE)

$(BIN_DIR)/test: $(SOURCE_FILES) $(SECRET_KEY_FILE) $(TEST_FILES) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SOURCE_FILES) $(SECRET_KEY_FILE) $(TEST_FILES)

test: $(BIN_DIR)/test
	MallocNanoZone=0 $<

serve: $(BIN_DIR)/serve
	# Tell the OS itself to ignore any incoming traffic on my chosen local port - I want to handle that myself!
	iptables -t filter --append INPUT --protocol tcp --dport $(LOCAL_PORT) -j DROP
	# disable MallocNanoZone as a fix for this issue on macOS
	# https://stackoverflow.com/questions/69861144/get-an-error-as-a-out40780-0x1130af600-malloc-nano-zone-abandoned-due-to-in
	-MallocNanoZone=0 $<
	# Wait until all traffic is done, then revert the previous iptables change
	sleep 10
	iptables -t filter --delete INPUT --protocol tcp --dport $(LOCAL_PORT) -j DROP

clean:
	rm -rf bin $(DEFINES_FILE) $(SECRET_KEY_FILE)