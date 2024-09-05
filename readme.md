# Proyecto shell custom
## Integrantes del grupo
- Carlos Tomás Álvarez Norambuena  (*2022433621*)
- Antonio  Jesus Benavides Puentes (*2023455954*)
- Javier Alejandro Campos Contreras (*2023432857*)
## Instrucciones de compilacion

### Version de compilador recomendada y sistemas testeados
El codigo esta compilado con gcc en linux. Las distribuciones utilizadas en el testeo son Manjaro y Kali linux. 

Las version de gcc recomendada para compilar el proyecto es *gcc (GCC) 14.2.1*. 
### Comandos para ejecutar el código 
Para compilarlo simplemente ejecutar ``` make``` despues de clonar el repositorio en el directorio del mismo.

Si make llegase a fallar se puede ejecutar el comando manualmente con:

``` gcc -I ./include -o shell.out ./src/interfaz.c ./src/signal_handlers.c ./src/main.c ./src/comandos.c ./src/utils.c ./src/favs.c ```

Después de compilarlo simplemente  ejecutar ``` ./shell ``` en el directorio del proyecto.

