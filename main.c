#include "command.h"
#include "kvs.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

int call_lua_function(lua_State *L, const char *function_name,
                      const char *param, char *error_message) {
  lua_getglobal(L, function_name);
  lua_pushstring(L, param);

  if (lua_pcall(L, 1, 2, 0) != 0) {
    strcpy(error_message, lua_tostring(L, -1));
    lua_pop(L, 1);
    return 0;
  }

  if (lua_isboolean(L, -2)) {
    int success = lua_toboolean(L, -2);
    if (!success) {
      strcpy(error_message, lua_tostring(L, -1));
      lua_pop(L, 2);
      return 0;
    } else {
      lua_pop(L, 2);
      return 1;
    }
  }

  strcpy(error_message, "Erro inesperado na função Lua");
  lua_pop(L, 2);
  return 0;
}

int call_lua_function_with_result(lua_State *L, const char *function_name,
                                  const char *param, char *result,
                                  char *error_message) {
  lua_getglobal(L, function_name);
  lua_pushstring(L, param);

  if (lua_pcall(L, 1, 1, 0) != 0) {
    strcpy(error_message, lua_tostring(L, -1));
    lua_pop(L, 1);
    return 0;
  }

  if (lua_isstring(L, -1)) {
    strcpy(result, lua_tostring(L, -1));
    lua_pop(L, 1);
    return 1;
  }

  strcpy(error_message, "Erro ao obter resultado da função Lua");
  lua_pop(L, 1);
  return 0;
}

int main(int argc, char *argv[]) {
  char input[100];
  KVSstore *banco;
  char result[100];
  char error_message[100];

  // Inicializando KVS
  banco = kvs_create();

  // Inicializando Lua
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  // Carregando o script Lua
  if (luaL_dofile(L, "extensions.lua")) {
    printf("Erro ao carregar script Lua: %s\n", lua_tostring(L, -1));
    return 1;
  }

  printf("Execute algum comando. Exemplos:\n");
  printf("- ADD abc 1234 ---> Adiciona o valor 1234 na chave abc\n");
  printf("- GET abc ---> Retorna o valor da chave abc se existir\n");
  printf("- EXIT ---> sair do programa\n");

  for (;;) {
    // Imprime e mantém na mesma linha
    printf("> ");
    fflush(stdout);
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0'; // Remove o newline

    Command command = get_command(input);

    if (!strcmp(command.command, "EXIT")) {
      printf("Inté!\n");
      break;
    } else if (!strcmp(command.command, "ADD")) {
      printf("Adicionando chave %s com valor %s\n", command.key,
             command.value); // Debug
      if (strncmp(command.key, "cpf_", 4) == 0) {
        // Validação de CPF
        if (call_lua_function(L, "validate_cpf", command.value,
                              error_message)) {
          kvs_put(banco, command.key, command.value);
          printf("CPF adicionado com sucesso.\n");
        } else {
          printf("Erro ao adicionar CPF: %s\n", error_message);
        }
      } else if (strncmp(command.key, "data_", 5) == 0) {
        // Validação de Data
        if (call_lua_function(L, "validate_date", command.value,
                              error_message)) {
          kvs_put(banco, command.key, command.value);
          printf("Data adicionada com sucesso.\n");
        } else {
          printf("Erro ao adicionar data: %s\n", error_message);
        }
      } else {
        // Comando padrão
        kvs_put(banco, command.key, command.value);
        printf("Valor adicionado com sucesso.\n");
      }
    } else if (!strcmp(command.command, "GET")) {
      char *value = (char *)kvs_get(banco, command.key);
      printf("Recuperando valor para chave %s: %s\n", command.key,
             value ? value : "NULL"); // Debug
      if (value) {
        if (strncmp(command.key, "cpf_", 4) == 0) {
          if (call_lua_function_with_result(L, "format_cpf", value, result,
                                            error_message)) {
            printf("%s => %s\n", command.key, result);
          } else {
            printf("Erro ao formatar CPF: %s\n", error_message);
          }
        } else if (strncmp(command.key, "data_", 5) == 0) {
          if (call_lua_function_with_result(L, "format_date", value, result,
                                            error_message)) {
            printf("%s => %s\n", command.key, result);
          } else {
            printf("Erro ao formatar data: %s\n", error_message);
          }
        } else {
          printf("%s => %s\n", command.key, value);
        }
      } else {
        printf("Chave %s não encontrada\n", command.key);
      }
    } else {
      printf("Comando não reconhecido.\n");
    }
  }

  kvs_destroy(banco);
  lua_close(L);
  return 0;
}
