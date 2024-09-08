# Nombre del ejecutable
TARGET = shell.out

# Compilador
CC = gcc

# Opciones de compilación
CFLAGS = -I ./include -g -Wall

# Directorio para archivos objeto
OBJ_DIR = build

# Archivos fuente
SOURCES = ./src/interfaz.c ./src/signal_handlers.c ./src/main.c ./src/comandos.c ./src/utils.c ./src/favs.c ./src/memoria_comandos.c ./src/internos_comandos.c ./src/externos_comandos.c

# Archivos objeto (guardados en build/)
OBJECTS = $(patsubst ./src/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

.PHONY: all clean run debug memoria

# Regla por defecto: compilar el programa
all: $(TARGET)

# Regla para compilar el programa
$(TARGET): $(OBJECTS)
	@$(CC) $(OBJECTS) -o $(TARGET) $(CFLAGS)

# Regla para compilar los archivos objeto y guardarlos en obj/
$(OBJ_DIR)/%.o: ./src/%.c | $(OBJ_DIR)
	@$(CC) -c $< -o $@ $(CFLAGS)

# Crear el directorio obj si no existe
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Regla para ejecutar el programa
run: $(TARGET)
	@./$(TARGET)

# Regla para debuggear el programa
debug: $(TARGET)
	@gdb ./$(TARGET)

# Regla para investigar las fugas de memoria
memoria: $(TARGET)
	@valgrind --leak-check=full ./$(TARGET)

# Regla para limpiar los archivos objeto y el ejecutable
clean:
	@rm -rf $(OBJ_DIR) $(TARGET)
