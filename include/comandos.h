#pragma once
#include "internos_comandos.h"
#include "externos_comandos.h"


extern char ***comandos;

char ***entrada_comandos();

void liberar_comandos();

void liberar_cache();

void guardar_comandos();

