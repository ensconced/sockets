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
	-std=c17 \
	-fsanitize=address,undefined \
	-g \
	-D_POSIX_C_SOURCE=199309L \
	-lssl \
	-lcrypto \
	-O1

BIN_DIR=bin

clean:
	rm -r bin