#pragma once

extern char ***comandos;
extern char ***comandos_anteriores;

char ***entrada_comandos();

void liberar_comandos();

void liberar_comandos_anteriores();

void manejar_comandos_externos(char **comando);
int manejar_comandos_internos(char **comando);
void guardar_comandos();

//Funciones de favs
void guardar_cache();
void eliminar_cache();
void guardar_favs();
void eliminar_favs();
void mostrar_favs();
void cargar_favs();