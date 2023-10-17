# REDES-I-IMPLEMENTACION-PROTOCOLO-HTTP-SOCKETS

# Implementación del protocolo HTTP simplificado mediante el uso de sockets en C

![descarga](https://github.com/rmelgo/REDES-I-IMPLEMENTACION-PROTOCOLO-HTTP-SOCKETS/assets/145989723/d3dca059-14fa-4055-8425-458505a245af)

# - Introducción

Proyecto realizado en la asignatura de Redes de Computadores I del grado de Ingenieria Informática de la Universidad de Salamanca. El enunciado del proyecto se encuentra subido en el repositorio en un archivo pdf llamado *Sockets2021_G.pdf*.
  
El principal objetivo de este proyecto es la realización de un programa en C que implemente un versión reducida del protocolo HTTP utilizando la API de sockets de Berkeley, siguiendo las especificaciones descritas en *Sockets2021_G.pdf*.
De esta manera, el objetivo principal no es la programación sino el aprendizaje del uso del los sockets como los protocolos de comunicación TCP y UDP.

El proyecto incluye un documento llamado *Memoria.pdf* que contiene una breve explicación del código implementado así como unas pruebas de ejecución del correcto funcionamiento del proyecto.

# - Comentarios sobre el entorno de ejecución

Para ejecutar este programa, se requerira de una distribución del Sistema Operativo **GNU/Linux**.    

Para poder compilar correctamente el programa, se deberá tener instalada una version del compilador gcc o similar. En el caso de no tener gcc, se puede instalar facilmente con el siguiente comando:

```sudo apt install gcc```

# - Funcionamiento del proyecto

El protocolo HTTP se trata de un protocolo con un modelo de arquitectura **cliente-servidor**. De esta manera, el proyecto contará con una aplicación cliente (*"cliente.c"*) y una aplicación servidor (*"servidor.c"*).

El protocolo HTTP cuenta con 2 tipos de mensajes:
- Peticiones de los clientes al servidor
- Respuestas del servidor a los clientes

## Peticiones de los clientes al servidor

Las peticiones de los clientes al servidor deben tener la siguiente estructura: 

```GET <nombre-página-web> <versión-HTTP> <CR><LF>```  
```Host: <nombre-host-servidor> <CR><LF>```   
```Connection: <tipo-conexión> <CR><LF>```  
```<CR><LF>```

Significado de los campos:
- ***<nombre-página-web>*** hace referencia al nombre de la página web de la cual se desea recuperar su contenido.
- ***<versión-HTTP>*** hace referencia a la versión del protocolo HTTP utilizada.
- **<*nombre-host-servidor*>** hace referencia al nombre de la máquina del servidor.
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

## Tareas realizadas por el servidor

El servidor debe estar disponible para atender disintas peticiones de clientes, por lo que se ejecutará permanentemente en segundo plano como un ***proceso daemon***.

El servidor aceptará peticiones de clientes tanto en TCP como en UDP.

Cada vez que reciba una peticion GET, el servidor buscará en la carpeta ***Paginas_web*** el contenido de la página web solicitada. En esta carpeta se pueden ir añadiendo disintas páginas web de prueba para comprobar el correcto funcionamiento del proyecto.

Adicionalmente, el servidor creará un fichero llamado ***peticiones.log*** en el que registrará todos los eventos que se produzcan. Para cada evento se registrará la siguiente información:

- **Fecha y hora** en la que se produce el evento
- **Nombre del ejecutable** que registra el evento
- **Descripción del evento**: Esta descripción variará en función del tipo de evento. Se distiguen 3 tipos de eventos:
  
  - **Comunicación realizada**: Este evento se produce cuando se establece una nueva comunicación con un nuevo cliente. Para este evento se registrará la siguente información:
    
      - Nombre o hostname del equipo cliente
      - Dirección IP del equipo cliente
      - Protocolo de transporte utilizado para la comunicación (TCP o UDP)
      - Número del puerto efímero utilizado por el cliente
  - **Operación**: Este evento se produce cuando cuando el servidor responde a una petición realizada por un cliente. Para este evento se registrará un mensaje indicando la correcta atención a la petición si se ha atendido correctamente o la causa de error si la petición no se ha podido atender correctamente.
  - **Comunicación finalizada**: Este evento se produce cuando finaliza una comunicación con un cliente. Para este evento se registrará la siguente información:
    
      - Nombre o hostname del equipo cliente
      - Dirección IP del equipo cliente
      - Protocolo de transporte utilizado para la comunicación (TCP o UDP)
      - Número del puerto efímero utilizado por el cliente
   
En la siguiente imagen se adjunta un ejemplo de como puede quedar el fichero ***peticiones.log***:

![Ejemplo log con borde](https://github.com/rmelgo/REDES-I-IMPLEMENTACION-PROTOCOLO-HTTP-SOCKETS/assets/145989723/fb35365b-457c-4e23-8768-e4098f921b2b)

## Tareas realizadas por el cliente

Realizará peticiones al servidor utilizando los protocolos de transporte TCP o UDP.

Para poder funcionar correctamente, se debe especificar en la ejecución del cliente los siguientes parámetros:

- El nombre (hostname) o dirección IP del servidor
- El protocolo de transporte utilizado para la comunicación (TCP o UDP)
- Un fichero de texto donde se encuentran las peticiones u ordenes que debe realizar el cliente.

Las peticiones dentro del fichero deben tener la siguiente estructura:

```<nombre-petición> <nombre-página-web> <k/c>```  

Significado de los campos:

- ***<nombre-petición>*** hace referencia al nombre de la petición que se desea realizar.
- ***<versión-HTTP>*** hace referencia a la versión del protocolo HTTP utilizada.
- **<*k/c*>** indica como debe ser el estado de la conexión despues de realizar la petición. En el caso de utilizar el protocolo TCP, una k significa que se desea mantener la conexión con el servidor tras realizar la petición y una c significa que se desea finalizar la conexión con el servidor una vez realizada la petición. En el protocolo UDP este campo no es necesario ya que no se establece ningún tipo de conexión.

### Ejemplo de petición del cliente:

```GET /index.html k```  

### Ejemplo de comando de ejecución del cliente:

```cliente www.usal.es TCP ordenes.txt```  

### Registro de la actividad del cliente

Por otra parte, el contenido tanto de las peticiones realizas por el cliente como las respuestas recibidas por el servidor se almacenarán en un fichero de texto cuyo nombre será el número de puerto efímero utilizado por el cliente en dicha comunicación.

En la siguiente imagen, se muestra un ejemplo del contenido de dicho fichero de texto:

![Ejemplo puerto efimero con borde](https://github.com/rmelgo/REDES-I-IMPLEMENTACION-PROTOCOLO-HTTP-SOCKETS/assets/145989723/b314bdd0-cb00-4bd7-9ffb-01a01e9a6f45)

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
- Un fichero llamado ***peticiones.log*** que contiene un registro de las peticiones recibidas por el servidor mientras este se encontraba en mantenimiento. También se registran las comunicaciones establecidas y finalizadas con los distintos clientes.
- Una carpeta llamada ***Paginas_web*** que contiene distintas páginas web en formato HTML. De esta manera, cuendo el servidor reciba una petición HTTP sobre una página web determinada, este buscará dentro de dicha carpeta y proporcionara el contenido de dicha página web en el caso de que esta exista.

# - Pasos necesarios para ejecutar el programa

**Paso 1: Compilar el programa**  

Para ello se debe introducir el siguiente comando:    

```make```

Con este comando se compila tanto el código del cliente como el código del servidor, utilizando las directivas de compilación del fichero ***makefile***. El script ***lanzaServidor.sh*** también realiza este cometido.

**Paso 2: Ejecutar el programa**  

Para ello, se debe poner en ejecución al servidor en primer lugar con este simple comando:

```./servidor```

Después, se ejecutará el cliente con la sintaxis especificada anteriormente. Por ejemplo:

```cliente localhost TCP ordenes.txt```  

**Paso 3: Obtención de resultados**  

Se mostrará por la terminal el contenido de las respuestas del servidor en forma de contenido de páginas web o de mensajes de error. Se registrará en ***peticiones.log*** los eventos registrados por el servidor y se crearan los ficheros de depuración con el nombre del puerto efímero correspondientes.

**Finalización del programa**

Para finalizar la ejecución del cliente simplemente bastara con pulsar las teclas CTRL+C desde el terminal. De esta manera, el cliente automaticamente recibirá una señal de terminación por lo que realizará las tareas necsarias para finalizar el cliente. Sin embargo, el servidor seguira ejecutandose en segundo plano al tratarse de un proceso demonio.

Para finalizar la ejecución del servidor, se debe buscar el PID del proceso correspondiente y matarlo con la orden kill. También es posible acabar con dicho proceso a través del monitor de recursos de Linux.

# - Ejemplo de ejecución

## Ejemplo de ejecución con TCP

En primer lugar, se mostrará un ejemplo del funcionamiento del proyecto utilizando el protocolo TCP y el fichero *ordenes3.txt*.

Tras realizar la prueba, el resultado obtenido en la terminal es el siguiente:

![Ejemplo ejecucion TCP](https://github.com/rmelgo/REDES-I-IMPLEMENTACION-PROTOCOLO-HTTP-SOCKETS/assets/145989723/d71c66c2-ecae-4d9a-8722-0d3189b1ae61)

Por otro lado, el contenido del fichero del puerto efímero es el siguiente:

![Ejemplo ejecucion TCP efimero 1](https://github.com/rmelgo/REDES-I-IMPLEMENTACION-PROTOCOLO-HTTP-SOCKETS/assets/145989723/11d1c768-6102-453b-8f09-0c88dd6e8929)
![Ejemplo ejecucion TCP efimero 2](https://github.com/rmelgo/REDES-I-IMPLEMENTACION-PROTOCOLO-HTTP-SOCKETS/assets/145989723/4c865bb2-9653-4248-88fb-cfcc02281133)

## Ejemplo de ejecución con UDP

En segundo lugar, se mostrará un ejemplo del funcionamiento del proyecto utilizando el protocolo UDP y el fichero *ordenes3.txt*.

Tras realizar la prueba, el resultado obtenido en la terminal es el siguiente:

![Ejemplo ejecucion UDP](https://github.com/rmelgo/REDES-I-IMPLEMENTACION-PROTOCOLO-HTTP-SOCKETS/assets/145989723/82e996c6-0b0a-4155-b915-0341ca487abf)

Por otro lado, el contenido del fichero del puerto efímero es el siguiente:

![Ejemplo ejecucion UDP efimero](https://github.com/rmelgo/REDES-I-IMPLEMENTACION-PROTOCOLO-HTTP-SOCKETS/assets/145989723/cf6d3227-9e8d-47a9-9766-23caa01da105)

## Registro de las peticiones

En último lugar, se mostrará el contenido del fichero ***peticiones.log*** tras realizar estos 2 ejemplos. El contenido es el siguiente:

![Ejemplo ejecucion log](https://github.com/rmelgo/REDES-I-IMPLEMENTACION-PROTOCOLO-HTTP-SOCKETS/assets/145989723/91eb2660-aa4d-443c-acde-72e0ceb1785f)

**Nota**: Mirar líneas 159-180, el resto no tiene que ver con los ejemplos realizados en esta sección.
