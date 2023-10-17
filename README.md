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

# Comentarios sobre el material adjuntado

El proyecto cuenta con los siguientes ficheros:

- Un fichero llamado ***servidor.c*** que contendrá el código necesario para recibir las peticiones de los clientes tanto en ***TCP*** como ***UDP*** y responder a los clientes de manera adecuada, siguiendo las normas del protocolo HTTP.
- Un fichero llamado ***cliente.c*** que contendrá el código necesario para realizar peticiones al servidor tanto en ***TCP*** como ***UDP*** asi como el código necesario para recibir correctamente las respuestas suminsitradas por el servidor.
- Un fichero llamado ***makefile*** que contiene unas directivas de compilación básicas para generar un ejecutable tanto del programa cliente como del programa servidor.
- Un fichero llamado ***ordenes.txt*** que contiene un conjunto de peticiones HTTP las cuales serán leídas y ejecutadas por el cliente.
- Un fichero llamado ***ordenes1.txt*** que contiene un conjunto de peticiones HTTP las cuales serán leídas y ejecutadas por el cliente.
- Un fichero llamado ***ordenes2.txt*** que contiene un conjunto de peticiones HTTP las cuales serán leídas y ejecutadas por el cliente.
- Un fichero llamado ***ordenes3.txt*** que contiene un conjunto de peticiones HTTP las cuales serán leídas y ejecutadas por el cliente.
- Un fichero llamado ***lanzaServidor.sh*** el cual se trata de un script en bash que realiza las siguientes tareas:
  - Ejecutar el makefile de manera que se genere el ejecutable tanto del código del servidor como el código del cliente.
  - Lanzar el servidor, que se ejecutará como un proceso daemon y estará ejecutandose en segundo plano permanentemente.
  - Lanzar el cliente utilizando los distintos ficheros de ordenes tanto en el protocolo TCP como UDP.   

De esta manera, se consigue probar todos los tipos de peticiones HTTP posibles en los 2 protocolos de transporte (TCP y UDP) y probar como el servidor responde en cada una de estas situaciones.
- Un fichero llamado ***peticiones.log*** que contiene un registro de las peticiones recibidas por el servidor mientras este se encontraba en mantenimiento. También se registran las conexiones establecidas y finalizadas con los disintos clientes.
- Una carpeta llamada ***Paginas_web*** que contiene distintas páginas web en formato HTML. De esta manera, cuendo el servidor reciba una petición HTTP sobre una página web determinada, este buscará dentro de dicha carpeta y proporcionara el contenido de dicha página web en el caso de que esta exista.

# - Funcionamiento del proyecto

El protocolo HTTP se trata de un protocolo con un modelo de arquitectura **cliente-servidor**. De esta manera, el proyecto contará con una aplicación cliente (*"cliente.c"*) y una aplicación servidor (*"servidor.c"*).

El protocolo HTTP cuenta con 2 tipos de mensajes:
- Peticiones de los clientes al servidor
- Respuestas del servidor a los clientes

## Peticiones de los clientes al servidor

Las peticiones de los clientes al servidor deben tener la siguiente estructura: 

```GET <nombre-página-web> <versión-HTTP> <CR><LF>```  
```Host: <nombre-host-cliente> <CR><LF>```   
```Connection: <tipo-conexión> <CR><LF>```  
```<CR><LF>```

Significado de los campos:
- ***<nombre-página-web>*** hace referencia al nombre de la página web de la cual se desea recuperar su contenido.
- ***<versión-HTTP>*** hace referencia a la versión del protocolo HTTP utilizada.
- **<*nombre-host-cliente*>** hace referencia al nombre de la máquina del cliente.
- ***<tipo-conexión>*** hace referencia al tipo de conexión realizada con el cliente. Existen 2 tipos de conexiones:
  
  - **keep-alive**: La conexión del cliente con el servidor permanece abierta, permitiendo realizar varias peticiones seguidas sin cerrar la conexión con el servidor.
  - **close**: La conexión del cliente con el servidor se cierra cuando el servidor responda la petición del cliente. Si en el tipo de conexión, el campo aparece vacio, se interpretará como un close.

**Nota**: Cada linea de una petición HTTP terminan con un par de caracteres ***CR-LF*** donde ***CR*** es el carácter retorno de carro "\r" y ***LF*** es el carácter salto de línea "\n".

### Ejemplo de petición HTTP:

```GET /index.html HTTP/1.1 <CR><LF>```  
```Host: www.usal.es <CR><LF>```   
```Connection: keep-alive <CR><LF>```  
```<CR><LF>```

## Respuestas del servidor a los clientes

Las respuestas del servidor a los clientes deben tener la siguiente estructura: 

```<versión-HTTP> <código-respuesta> <CR><LF>```  
```Server: <nombre-host-servidor> <CR><LF>```   
```Connection: <tipo-conexión> <CR><LF>```  
```Content-Length: <longitud-respuesta> <CR><LF>```  
```<CR><LF>```
```<respuesta>```

Significado de los campos:
- ***<versión-HTTP>*** hace referencia a la versión del protocolo HTTP utilizada.
- ***<código-respuesta>*** hace referencia al tipo de respuesta realizada por el servidor. Existen 3 tipos de respuestas:
  
  - **200 OK**: La petición por parte del cliente se ha realizado correctamente y la página web por la que pregunta el cliente existe y es accesible por el servidor. En este caso, la respuesta incluye e
  - **404 Not Found**: La petición por parte del cliente se ha realizado correctamente pero la página web por la que pregunta el cliente no existe y no es accesible por el servidor.
  - **501 Not Implemented**: La petición por parte del cliente no se ha realizado correctamente.
  
- **<*nombre-host-servidor*>** hace referencia al nombre de la máquina del servidor.
- ***<tipo-conexión>*** hace referencia al tipo de conexión realizada con el cliente. Existen 2 tipos de conexiones:

  - **keep-alive**: La conexión del cliente con el servidor permanece abierta, permitiendo realizar varias peticiones seguidas sin cerrar la conexión con el servidor.
  - **close**: La conexión del cliente con el servidor se cierra cuando el servidor responda la petición del cliente. Si en el tipo de conexión, el campo aparece vacio, se interpretará como un close.
  
- **<*longitud-respuesta*>** hace referencia al número de carácteres de la respuesta que se adjunta a continuación.
- **<*respuesta*>** dependerá del código de respuesta.
  - Si el código de respuesta es **200 OK** este campo incluye el contenido de la página web que solicito el cliente.
  - Si el código de respuesta es **404 Not Found** este campo incluye el siguiente mensaje:  
    ```<html><body><h1>404 Not found</h1></body></html>```
  - Si el código de respuesta es **501 Not Implemented** este campo incluye el siguiente mensaje:  
    ```<html><body><h1>501 Not Implemented</h1></body></html>```
    
**Nota**: Cada linea de una petición HTTP terminan con un par de caracteres ***CR-LF*** donde ***CR*** es el carácter retorno de carro "\r" y ***LF*** es el carácter salto de línea "\n".

### Ejemplo de respuesta HTTP correcta:

```HTTP/1.1 200 OK <CR><LF>```  
```Server: www.usal.es <CR><LF>```   
```Connection: keep-alive <CR><LF>```  
```Content-Length: 59 <CR><LF>```  
```<CR><LF>```
```<html><body><h1>Universidad de Salamanca</h1></body></html>```

### Ejemplo de respuesta HTTP incorrecta por página inexistente:

```HTTP/1.1 404 Not Found <CR><LF>```  
```Server: www.usal.es <CR><LF>```   
```Connection: keep-alive <CR><LF>```  
```Content-Length: 48 <CR><LF>```  
```<CR><LF>```
```<html><body><h1>404 Not found</h1></body></html>```

### Ejemplo de respuesta HTTP incorrecta por orden errónea:

```HTTP/1.1 501 Not Implemented <CR><LF>```  
```Server: www.usal.es <CR><LF>```   
```Connection: keep-alive <CR><LF>```  
```Content-Length: 56 <CR><LF>```  
```<CR><LF>```
```<html><body><h1> 501 Not Implemented </h1></body></html>```
  
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


