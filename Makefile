.PHONY: all run debug memoria

# Regla para compilar el programa
all:
	@gcc -I ./include -o shell.out -g ./src/interfaz.c ./src/signal_handlers.c ./src/main.c ./src/comandos.c ./src/utils.c ./src/favs.c ./src/memoria_comandos.c ./src/internos_comandos.c ./src/externos_comandos.c

# Regla para ejecutar el programa
run: ./shell.out
	@./shell.out
# Regla para debugear el programa
debug: ./shell.out
	@gdb ./shell.out
# Regla para investigar las fugas de memoria
memoria: ./shell.out
	@valgrind --leak-check=full ./shell.out