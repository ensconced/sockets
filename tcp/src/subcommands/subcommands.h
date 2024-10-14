#pragma once

#define SUBCOMMAND_COUNT 4

typedef struct subcommand {
  const char *name;
  int (*handler)(int argc, char **argv);
  const char *help_text;
} subcommand;

extern subcommand subcommands[];
