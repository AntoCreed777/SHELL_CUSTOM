all:
	@gcc -I ./include -o shell.out ./src/interfaz.c ./src/signal_handlers.c ./src/main.c ./src/comandos.c ./src/utils.c ./src/favs.c ./src/memoria_comandos.c