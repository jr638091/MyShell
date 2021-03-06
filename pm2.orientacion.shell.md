Orientacion del Proyecto
========================

El objetivo del proyecto es hacer un shell que simule los shells del sistema operativo linux.

Las funcionalidades son las siguiente:

1. Al ejecutar el programa este debe imprimir un prompt (ejemplo "$ ")
2. Ejecutar comandos e imprimir su salida
3. Poder ejecutar el comando `cd` (y que cambie de directorio)
4. Redirigir entrada y salida estandar de comandos hacia/desde ficheros con `>`, `<`, `>>`
5. Redirigir la salida estandar de un comando hacia la entrada de otro con un pipe `|`
6. Una vez terminado el comando previo, se debe mostrar el prompt nuevamente.
7. Solamente habrá un espacio entre cada uno de los *tokens* de la entrada
  * comandos
  * parámetros
  * operadores de redirección (`>`, `|`, etc.)

y hasta aqui tienen 3 puntos en el proyecto

8. Implementar mas de una tubería (1pt adicional)

9. Implementar el operador `&` y tener procesos en el background (0.5pt adicional)
  * jobs => listar todos los procesos que estan correiendo en el background
  * fg <pid> => envia el proceso <pid> hacia el foreground
  * NOTA: aqui tiene que funcionar algo como `command1 < file1 | command2 > file2 &`

9. Permitir cualquier cantidad de espacios o no entre los comandos y parametros (0.5pt adicional)

10. Implementar un historial de comandos (un commando `history`) que permita imprimir todos los comandos ejecutados (1pt adicional)
  * al ejecutar `history` se debe imprimir al lado de cada comando anterior un numero consecutivo.
  * ejemplo
    1: ls
    2: cd /home
    3: mplayer game_of_thones_8x01.mkv
  * la historia debe almacenarse (i.e. si cierro el shell y lo vuelvo a abrir, la historia debe estar ahi)
  * implementar además un comando `again <number>` que vuelve a ejecutar el comando <number> de la historia
  * NOTA: el comando again lo que guarda en la historia es el comando que ejecutó
  * ejemplo:
    $ ls
    $ cd /home
    $ gcc p1.c -o p1.out
    $ history
      1: ls
      2: cd /home
      3: gcc p1.c -o p1.out
      4: history
    $ again 1
    # aqui ejecuta  "ls"
    $ history
      1: ls
      2: cd /home
      3: gcc p1.c -o p1.out
      4: history
      5: ls
      6: history
  * NOTA: si el comando empieza con espacio, entonces no va a parar al historial.


