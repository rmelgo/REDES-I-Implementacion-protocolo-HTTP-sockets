# REDES-I-IMPLEMENTACION-PROTOCOLO-HTTP-SOCKETS

# Implementación del protocolo HTTP simplificado mediante el uso de sockets en C

![descarga](https://github.com/rmelgo/REDES-I-IMPLEMENTACION-PROTOCOLO-HTTP-SOCKETS/assets/145989723/d3dca059-14fa-4055-8425-458505a245af)

# - Introducción

Proyecto realizado en la asignatura de Redes de Computadores I del grado de Ingenieria Informática de la Universidad de Salamanca. El enunciado del proyecto se encuentra subido en el repositorio en un archivo pdf llamado *Sockets2021_G.pdf*.
  
El principal objetivo de este proyecto es la realización de un programa en C que implemente un versión reducida del protocolo HTTP utilizando la API de sockets de Berkeley, siguiendo las especificaciones descritas en *Sockets2021_G.pdf*.
De esta manera, el objetivo principal no es la programación sino el aprendizaje del uso del los sockets como los protocolos de comunicación TCP y UDP.

# - Comentarios sobre el entorno de ejecución

Para ejecutar este programa, se requerira de una distribución del Sistema Operativo **GNU/Linux**.    

Para poder compilar correctamente el programa, se deberá tener instalada una version del compilador gcc o similar. En el caso de no tener gcc, se puede instalar facilmente con el siguiente comando:

```sudo apt install gcc```

# - Parámetros de ejecución

Para ejecutar el programa es necesario proporcionar 2 argumentos. 

El primer argumento se trata del **número máximo de procesos** (peatones y coches) que pueden existir de manera simultanea en el cruce. Este primer argumento puede tomar valores del ***3 al 49***, en caso contrario la practica no funcionará.

El segundo argumento hace referencia a la **velocidad** con la que se desplazaran los peatones y los coches. Este argumento podrá tomar valores del ***0 al 40*** de forma que la velocidad del programa será mas rápida cuanto mas pequeño sea el número introducido.  
Si el segundo argumento es 0, el programa se ejecutará a ***máxima velocidad***, lo que implicará un alto consumo de CPU.

Si los parámetros introducidos no respetan las reglas anteriores, el programa lo detectará, informará al usuario y acabará.

# Comentarios sobre el material adjuntado

El proyecto cuenta con los siguientes ficheros:

- Un fichero llamado ***servidor.c*** que contendrá el código necesario para recibir las peticiones del los clientes tanto en ***TCP*** como ***UDP*** y responder a los clientes de manera adecuada, siguiendo las nromas del protocolo HTTP.
- Un fichero llamado ***cliente.c*** que contendrá el código necesario para realizar peticiones al servidor tanto en ***TCP*** como ***UDP*** asi como el codigo necesario para recibir correctamente las respuestas suminsitradas por el servidor.
- Un fichero llamado ***makefile*** que contiene unas directivas de compilación básicas para generar un ejecutable tanto del programa cliente como del programa servidor.
- Una carpeta llamada ***Paginas_web***
  
# - Pasos necesarios para ejecutar el programa

**Paso 1: Compilar el programa**  

Para ello se debe introducir el siguiente comando:    

```gcc cruce.c libcruce.a -o cruce```

Como la libreria *libcruce.a* esta diseñada para sistemas de 32 bits y no es posible mezclar código de 64 bits con codigo de 32 bits, será necesario incluir una nueva directiva en el comando gcc para que genere codigo en 32 bits compatible con la biblioteca. De esta manera, el comando necesario para compilar el programa es el siguiente:

```gcc -m32 cruce.c libcruce.a -o cruce```

Si se produce algún tipo de error al realizar la compilación será por que el sistema donde se ejecuta el programa no tiene las librerias de 32 bits necesarias. Para incluirlas se deberá ejecutar el siguyiente comando:

```sudo apt-get install gcc-multilib```

Tras ejecutar este comando, se generará un fichero ejecutable llamado *cruce*. Observese como es necesario tanto el fichero *cruce.c* como la biblioteca estática de funciones *libcruce.a* para generar el ejecutable.

**Paso 2: Ejecutar el programa**  

Para ello se debe introducir el siguiente comando:    

```./cruce <número-máximo-procesos> <velocidad>```

Tras ejecutar este comando, el programa se habra ejecutado correctamente, siempre y cuendo se hayan introducido los argumentos correspondientes.

**Finalización del programa**

Para finalizar la ejecución del programa simplemente bastara con pulsar las teclas CTRL+C desde el terminal. De esta manera, el programa automaticamente recibira una señal de terminación por lo que realizará las tareas necsarias para finalizar el cruce y liberar todos los reccursos utilizados.

# - Ejemplo de ejecución


