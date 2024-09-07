#pragma once
#include <stdlib.h>
#include <stdbool.h>


bool is_empty_linea(char* linea);

char *eliminar_tabs(char *linea);

bool is_in_cache(char** comandos, char*** cache_comandos);

