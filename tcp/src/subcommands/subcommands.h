#pragma once

#include "./help/help.h"
#include "./start/start.h"
#include "./stop/stop.h"

#define SUBCOMMAND_COUNT 3

typedef struct subcommand {
  const char *name;
  int (*handler)(int argc, char **argv);
  const char *help_text;
} subcommand;

extern subcommand subcommands[];
