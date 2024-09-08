#pragma once
#include "internos_comandos.h"
#include "externos_comandos.h"
#include "memoria_comandos.h"


extern char ***comandos;
extern char ***comandos_anteriores;

char ***entrada_comandos();

void guardar_comandos_cache();

void guardar_comandos_anteriores();

void cargar_ruta_direccion_favs();