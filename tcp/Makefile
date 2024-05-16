include ../common.mk

# TODO - do I need to use sort/uniq here to avoid pulling in secret_key twice?
SOURCE_FILES = $(shell find . -name '*.c' ! -name '*.test.c' ! -name main.c) src/isn_generation/secret_key.c
TEST_FILES = $(shell find . -name '*.test.c')
MAIN_FILE = src/main.c

src/isn_generation/secret_key.c:
	head -c 16 /dev/random > /tmp/rand
	xxd -i -n secret_key /tmp/rand > src/isn_generation/secret_key.c
	rm /tmp/rand

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/serve: $(SOURCE_FILES) $(MAIN_FILE) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/test: $(SOURCE_FILES) $(TEST_FILES) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

test: $(BIN_DIR)/test
	MallocNanoZone=0 $<

serve: $(BIN_DIR)/serve
	# Tell the OS itself to ignore any incoming traffic on port 3001 - I want to handle that myself!
	iptables -t filter --append INPUT --protocol tcp --dport 3001 -j DROP
	# disable MallocNanoZone as a fix for this issue on macOS
	# https://stackoverflow.com/questions/69861144/get-an-error-as-a-out40780-0x1130af600-malloc-nano-zone-abandoned-due-to-in
	-MallocNanoZone=0 $<
	# Revert the previous iptables change
	iptables -t filter --delete INPUT --protocol tcp --dport 3001 -j DROP
