#define ERR_BUF_LEN 256

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

_Thread_local char err_buf[ERR_BUF_LEN];

void *checked_malloc(size_t size, char *id) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    if (strerror_r(errno, err_buf, ERR_BUF_LEN) == 0) {
      fprintf(stderr, "Failed to malloc %s: %s\n", id, err_buf);
    } else {
      fprintf(stderr, "Failed to malloc %s\n", id);
    }
    exit(1);
  }
  return ptr;
}

void *checked_calloc(size_t count, size_t size, char *id) {
  void *ptr = calloc(count, size);
  if (ptr == NULL) {
    if (strerror_r(errno, err_buf, ERR_BUF_LEN) == 0) {
      fprintf(stderr, "Failed to calloc %s: %s\n", id, err_buf);
    } else {
      fprintf(stderr, "Failed to calloc %s\n", id);
    }
    exit(1);
  }
  return ptr;
}
