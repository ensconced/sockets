CC=clang
CFLAGS=-Wall \
	-Wextra \
	-Wconversion \
	-Wshorten-64-to-32 \
	-Wconstant-conversion \
	-Wliteral-range \
	-Woverflow \
	-Wimplicit-int \
	-Wreserved-identifier \
	-pedantic-errors \
	-Werror \
	-std=c17 \
	-fsanitize=address,undefined \
	-g \
	-O1

BIN_DIR=bin

SOURCES = $(shell echo ./*.c)

$(BIN_DIR)/test: $(SOURCES)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) -o $@ $^

test: $(BIN_DIR)/test
	@$<
