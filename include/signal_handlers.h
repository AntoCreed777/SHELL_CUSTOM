#pragma once
#include "colores.h"

void sig_handler(int sig);


//A continuacion se detallan las diferentes funciones para manejar las distintas señales
void sigterm_handler(int sig);
void sigint_handler(int sig);

void sigchld_handler(int sig);

