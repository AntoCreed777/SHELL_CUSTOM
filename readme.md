

# Proyecto shell custom
## Integrantes del grupo
- [Carlos Tomás Álvarez Norambuena](https://github.com/Karantooo)  (*2022433621*)
- [Antonio  Jesus Benavides Puentes](https://github.com/AntoCreed777) (*2023455954*)
- [Javier Alejandro Campos Contreras](https://github.com/huebitoo) (*2023432857*)

## Tecnologias utilizadas en el proyecto
<p align="center">
  <a href="https://skillicons.dev">
    <img src="https://skillicons.dev/icons?i=git,github,vscode,bash,linux&perline=12" />
  </a>
</p>
<p align="center">
  <a href="https://skillicons.dev">
    <img src="https://skillicons.dev/icons?i=c&perline=12" />
  </a>
</p>


## Instrucciones de compilacion

### Version de compilador recomendada y sistemas testeados
El codigo esta compilado con gcc en linux. Las distribuciones utilizadas en el testeo son Manjaro y Windows subsystem for linux (WSL) Ubuntu.

Las version de gcc recomendada para compilar el proyecto es *gcc (GCC) 14.2.1*. 
### Comandos para ejecutar el código 
Para compilarlo y ejecutarlo simplemente ejecute ```make run``` despues de clonar el repositorio en el directorio del mismo.

Si make llegase a fallar se puede ejecutar el comando manualmente con:

```gcc -I ./include -o shell.out ./src/interfaz.c ./src/signal_handlers.c ./src/main.c ./src/comandos.c ./src/utils.c ./src/favs.c ./src/memoria_comandos.c ./src/internos_comandos.c ./src/externos_comandos.c```

Después de compilarlo simplemente ejecutar ```./shell.out ``` en el directorio del proyecto.

## Comandos propios
Dentro de la ejecucion de nuestra shell ejecutamos distintos comandos propios. Dentro de estos los que debemos destacar son los de favoritos y los de set recordatorio.

Vamos a dejar las variables que tiene que ingresar el usuario bajo la estructura ```$(variable)```.

### Comando: set recordatorio
Establece un recordatorio que se activara después de cierto tiempo. El tiempo se establece en segundos.

Para set recordatorio la estructura es la siguiente:

```set recordatorio $(tiempo) $(mensaje)```
#### Ejemplo
```set recordatorio 10 hola mundo```

Esto creara un recordatorio en 10 segundos con el mensaje hola mundo.


### Comando favs

Este comando se encarga de gestionar los comandos favoritos. Los comandos favoritos se añaden a la lista automaticamente cuando se ejecutan por primera vez en la terminal. Las diferentes opciones de este comando son:

 - help 
 - crear  ruta/ejemplo.csv
 - mostrar
 - eliminar num1, num2,...
 - buscar cmd
 - borrar
 - ejecutar num
 - cargar
 - guardar

#### Ejemplo 1

```favs mostrar```

Esto muestra la lista de comandos favoritos.

#### Ejemplo 2
Vale recalcar que algunos comandos en esta lista requieren de argumentos.

```favs eliminar 2 4 3```

Esto eliminar el segundo, cuarto y tercer elementos de la lista de favoritos que se ve en mostrar.

#### Ejemplo 3
Es necesario mencionar que en el comando ```ejecutar``` se modifico el funcionamiento con respecto al enunciado, en donde se invirtio el orden de los argumentos.

```favs ejecutar 1```

Esto ejecutara el primer elemento de la lista de favoritos

