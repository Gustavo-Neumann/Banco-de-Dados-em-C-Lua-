#include "kvs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const size_t kvs_pair_size = sizeof(KVSpair);
static const size_t kvs_store_size = sizeof(KVSstore);

// Função auxiliar para comparar pares ao ordenar
static int kvs_sort_compare(const void *a, const void *b) {
  const KVSpair *pairA = a;
  const KVSpair *pairB = b;
  return strcmp(pairA->key, pairB->key);
}

// Função auxiliar para buscar pares
static int kvs_search_compare(const void *key, const void *element) {
  const KVSpair *pair = element;
  return strcmp((const char *)key, pair->key);
}

// Função para buscar um par chave-valor
static KVSpair *kvs_get_pair(KVSstore *store, const void *key) {
  if (!store || !store->pairs) {
    return NULL;
  }
  return bsearch(key, store->pairs, store->length, kvs_pair_size, kvs_search_compare);
}

// Redimensiona o array de pares
static void kvs_resize_pairs(KVSstore *store) {
  if (!store) {
    return;
  }
  store->pairs = realloc(store->pairs, kvs_pair_size * store->length);
}

// Ordena os pares após a adição de novos pares
static void kvs_sort_pairs(KVSstore *store) {
  if (!store || !store->pairs) {
    return;
  }
  qsort(store->pairs, store->length, kvs_pair_size, kvs_sort_compare);
}

// Cria um novo par chave-valor
static void kvs_create_pair(KVSstore *store, const void *key, void *value) {
  if (!store || !key || !value) {
    return;
  }

  // Aumenta o array de pares
  store->length++;
  kvs_resize_pairs(store);

  KVSpair *new_pair = &store->pairs[store->length - 1];
  new_pair->key = strdup((const char *)key);  // Duplicando a string para evitar problemas com ponteiros
  new_pair->value = strdup((char *)value);    // Copiando o valor

  kvs_sort_pairs(store);  // Ordena após a criação do par
}

// Remove um par chave-valor
static void kvs_remove_pair(KVSstore *store, KVSpair *pair) {
  if (!store || !pair) {
    return;
  }
  pair->key = NULL;
  kvs_sort_pairs(store);
  store->length--;
  kvs_resize_pairs(store);
}

// Cria o banco de dados
KVSstore *kvs_create(void) {
  KVSstore *store = malloc(kvs_store_size);
  store->pairs = NULL;
  store->length = 0;
  return store;
}

// Destroi o banco de dados, liberando a memória
void kvs_destroy(KVSstore *store) {
  if (store) {
    if (store->pairs) {
      for (size_t i = 0; i < store->length; i++) {
        free(store->pairs[i].key);
        free(store->pairs[i].value);
      }
      free(store->pairs);
    }
    free(store);
  }
}

// Insere ou atualiza um par chave-valor
void kvs_put(KVSstore *store, const char *key, char *value) {
  KVSpair *pair = kvs_get_pair(store, key);
  if (pair) {
    // Atualiza o valor existente
    free(pair->value);  // Libera o valor antigo
    pair->value = strdup(value);  // Copia o novo valor
  } else {
    // Cria um novo par
    kvs_create_pair(store, key, value);
  }
}

// Recupera um valor a partir de uma chave
void *kvs_get(KVSstore *store, const void *key) {
  KVSpair *pair = kvs_get_pair(store, key);
  return pair ? pair->value : NULL;
}
