#ifndef COMMAND_H
#define COMMAND_H

typedef struct {
  char *command;
  char *key;
  char *value;
} Command;

Command get_command(char *text);

#endif