#include "command.h"
#include <stdio.h>
#include <string.h>

// https://www.educative.io/answers/splitting-a-string-using-strtok-in-c
// https://wagnergaspar.com/como-dividir-uma-string-em-tokens-com-a-funcao-strtok-em-c/

Command get_command(char *text) {
  Command _command;

  _command.command = strtok(text, " ");

  if (!strcmp(_command.command, "ADD") || !strcmp(_command.command, "GET")) {
    _command.key = strtok(NULL, " ");

    if (!strcmp(_command.command, "ADD")) {
      _command.value = strtok(NULL, " ");
    }
  }

  return _command;
}