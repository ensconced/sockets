include ../common.mk

SOURCES = $(shell echo src/*.c) src/secret_key.c

src/secret_key.c:
	head -c 16 /dev/random > /tmp/rand
	xxd -i -n secret_key /tmp/rand > src/secret_key.c
	rm /tmp/rand

$(BIN_DIR)/serve: $(SOURCES)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) -o $@ $^

serve: $(BIN_DIR)/serve
	# disable MallocNanoZone as a fix for this issue on macOS
	# https://stackoverflow.com/questions/69861144/get-an-error-as-a-out40780-0x1130af600-malloc-nano-zone-abandoned-due-to-in
	@MallocNanoZone=0 $<
