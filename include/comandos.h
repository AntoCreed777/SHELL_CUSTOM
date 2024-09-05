#pragma once

extern char ***comandos;
extern char ***cache_comandos;

char ***entrada_comandos();

void liberar_comandos();

void liberar_cache();

void manejar_comandos_externos(char **comando);
int manejar_comandos_internos(char **comando);
void guardar_comandos();

//Funciones de favs
void guardar_favs();
void borrar_favs();
void mostrar_favs();
void cargar_favs();
void eliminar_favs(int numero);
void buscar_favs(char *busqueda);