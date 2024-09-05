#pragma once

extern char *archivo_favs;  //Archivo donde se guarda la lista de favoritos
extern const char *direccion_favs;  //Archivo donde se guarda la direccion del archivo de favoritos
extern char ***cache_comandos;
extern int indice_inicio_anterior_linea_comando;



void guardar_favs();
void borrar_favs();
void mostrar_favs();
void cargar_favs();
void eliminar_favs(int *numero, int numero_comandos_eliminar);
void buscar_favs(char *busqueda);
void ejecutar_favs(int numero);
void guardar_ruta_favs();
void cargar_ruta_favs();