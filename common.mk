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
	-std=c23 \
	-fsanitize=address,undefined \
	-g \
	-D_POSIX_C_SOURCE=200112L \
	-lssl \
	-lcrypto \
	-O0

BIN_DIR=bin
