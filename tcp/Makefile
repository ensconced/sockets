include ../common.mk

# TODO - do I need to use sort/uniq here to avoid pulling in secret_key twice?
SOURCES = $(shell find . -name '*.c' ! -name '*.test.c') src/isn_generation/secret_key.c
TEST_SOURCES = $(shell find . -name '*.test.c')

src/isn_generation/secret_key.c:
	head -c 16 /dev/random > /tmp/rand
	xxd -i -n secret_key /tmp/rand > src/isn_generation/secret_key.c
	rm /tmp/rand

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/serve: $(SOURCES) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/test: $(SOURCES) $(TEST_SOURCES) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

test: $(BIN_DIR)/test
	MallocNanoZone=0 $<

serve: $(BIN_DIR)/serve
	# disable MallocNanoZone as a fix for this issue on macOS
	# https://stackoverflow.com/questions/69861144/get-an-error-as-a-out40780-0x1130af600-malloc-nano-zone-abandoned-due-to-in
	MallocNanoZone=0 $<
