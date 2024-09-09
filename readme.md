# Proyecto shell custom
## Integrantes del grupo
- [Carlos Tomás Álvarez Norambuena](https://github.com/Karantooo)  **(2022433621)**
- [Antonio Jesus Benavides Puentes](https://github.com/AntoCreed777) **(2023455954)**
- [Javier Alejandro Campos Contreras](https://github.com/huebitoo) **(2023432857)**

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


## Instrucciones de compilación

### Versión de compilador recomendada y sistemas testeados
El código esta compilado con gcc en linux. Las distribuciones utilizadas en el testeo son Manjaro y Windows subsystem for linux (WSL) Ubuntu.

La versión de gcc recomendada para compilar el proyecto es *gcc (GCC) 14.2.1*. 
### Comandos para ejecutar el código 
Para compilarlo y ejecutarlo simplemente ejecute ```make run``` después de clonar el repositorio en el directorio del mismo.

Si make llegase a fallar se puede ejecutar el comando manualmente con:

```gcc -I ./include -o shell.out ./src/interfaz.c ./src/signal_handlers.c ./src/main.c ./src/comandos.c ./src/utils.c ./src/favs.c ./src/memoria_comandos.c ./src/internos_comandos.c ./src/externos_comandos.c```

Después de compilarlo simplemente ejecutar ```./shell.out ``` en el directorio del proyecto.

## Comandos propios
Dentro de la ejecución de nuestra shell ejecutamos distintos comandos propios. Dentro de estos los que debemos destacar son los de favoritos y los de set recordatorio.

Vamos a dejar las variables que tiene que ingresar el usuario bajo la estructura ```$(variable)```.

### Comando: set recordatorio
Establece un recordatorio que se activara después de cierto tiempo. El tiempo se establece en segundos.

Para set recordatorio la estructura es la siguiente:

```set recordatorio $(tiempo) $(mensaje)```
#### Ejemplo
```set recordatorio 10 hola mundo```

Esto creara un recordatorio en 10 segundos con el mensaje hola mundo.


### Comando favs

Este comando se encarga de gestionar los comandos favoritos. Los comandos favoritos se añaden a la lista automáticamente cuando se ejecutan por primera vez en la terminal. Las diferentes opciones de este comando son:

 - help 
 - crear ruta/ejemplo.csv
 - mostrar
 - eliminar num1, num2
 - buscar cmd
 - borrar
 - ejecutar num
 - cargar
 - guardar

 ### CAMBIOS E INTERPRETACIONES HECHAS EN NUESTRO PROYECTO
 <span style="color: red;">Dentro del enunciado se tuvieron que hacer interpretaciones y algunos cambios menores. A continuación, se detallan los mismos:</span>

 - En el enunciado aparece que la estructura del comando ejecutar era ```favs num ejecutar```. Nosotros lo dejamos como ```favs ejecutar num``` ya que consideramos que era más coherente con el comando eliminar. 

 - En el comando cargar se podía interpretar de distintas maneras. Nosotros lo interpretamos como que cargar no sobrescribe lo que hay en RAM sino que concatena el contenido del archivo a lo que ya está presente en RAM. Los elementos que ya están cargados en memoria no se terminan repitiendo.

 - En el enunciado aparecía ```favs eliminar num1, num2```. Nosotros lo interpretamos como que se podía tomar una cantidad no determinada de comandos y que eliminábamos los comandos con esos números. Otra interpretación con la que nos debatimos era que num1 y num2 fuera un límite superior e inferior, pero nos decantamos por la primera. 

- En el enunciado se dice "*Los comandos favoritos deben almacenarse en
alguna estructura de datos que contenga los comandos en memoria cuando la shell esté activa y en un archivo cuando la shell se cierra...*". En favs guardar consideramos que lo más lógico es que fuese la escritura en disco. Así los únicos comandos que escriben en disco son guardar, cargar y crear, el resto se guardan en la lista que tenemos en RAM.

#### Ejemplo 1

```favs mostrar```

Esto muestra la lista de comandos favoritos.

#### Ejemplo 2
Vale recalcar que algunos comandos en esta lista requieren de argumentos.

```favs eliminar 2 4 3```

Esto eliminar el segundo, cuarto y tercer elementos de la lista de favoritos que se ve en mostrar.

#### Ejemplo 3
Es necesario mencionar que en el comando ```ejecutar``` se modificó el funcionamiento con respecto al enunciado, en donde se invirtió el orden de los argumentos.

```favs ejecutar 1```

Esto ejecutara el primer elemento de la lista de favoritos. 

