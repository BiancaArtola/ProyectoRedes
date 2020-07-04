# Proyecto Redes de Computadoras 

Empleando el lenguaje de programación C, se implementa un programa con el propósito de servir de herramienta de consulta y exploración de la jerarquía DNS bajo el sistema operativo GNU/Linux. El mismo se ejecuta desde la consola e interactúa con uno o más servidores DNS, respetando siempre el intercambio de mensajes estipulado en los RFC 1034 y RFC 1035. Se utilizó este lenguaje de programación C ya que cuenta con la posibilidad de utilizar registros y punteros.

## Descripción
La entidad principal de nuestro sistema es el usuario que ingresa la expresión al sistema.
El usuario ingresa una expresión en el sistema por consola. Al ingresar la misma, el sistema comienza a trabajar sobre la misma: comienza evaluando si es válida. En caso de que no lo sea, se indica por pantalla un mensaje de ayuda, y se termina la ejecución de la misma.


### Prerequisitos e Instalación

1. Obtener los archivos consulta.c, ayuda.c, dnsquery.c, dns.c, socket.c, DNS-service.c , consultaLOC.c, con sus correspondientes archivos .h , ayuda.txt y el archivo script.sh.
2. Ingresar a Terminal, y una vez ubicados en el directorio donde se encuentran los archivos del paso 1) [Dentro de la carpeta “Archivos.C”] ingresar por consola: sh script.sh
3. Ejecutar el programa ingresando ./dns <consultaDNSquery>

### Ayuda del sistema

El siguiente mensaje de ayuda es provisto por el sistema:
<p align="center">
<img src="https://i.ibb.co/Z2cK1xn/Sin-t-tulo.png" alt="drawing" />
</p>

El “-h” puede ingresarse en cualquier parte de la consulta. Por ejemplo, las siguientes entradas son consideradas como válidas:
* ./dns query -h
* ./dns query www.google.com -h
* ./dns query www.google.com -a -h
* ./dns query www.google.com @8.8.8.8 -h
En dichos casos, SOLO se mostrará la ayuda del programa, ignorando la consulta realizada por el usuario, ya que se asume que el mismo está pidiendo ayuda para realizarla.

### Ejemplos de funcionamiento
Si el usuario ingresa la siguiente consulta: query google.com -mx -t, la salida será la siguiente:
<p align="center">
<img src="https://i.ibb.co/Z8hb5rG/Sin-t-tulo.png" alt="drawing"  />
</p>

En la respuesta a consultas de tipo recursivas, nuestro programa muestra la siguiente información:
<p align="center">
<img src="https://i.ibb.co/gFG94wd/Sin-t-tulo.png" alt="drawing" />
</p>

Donde:
* Questions indica la cantidad de consultas realizadas por el usuario. en nuestro caso, siempre será igual a 1.
* Answers indica la cantidad de respuestas de tipo “Answer” que el programa encontró para esa consulta.
* Authoritive indica la cantidad de respuestas de tipo “Authoritive” que el programa encontró para esa consulta.
* Additional indica la cantidad de respuestas de tipo “Additional” que el programa encontró para esa consulta.

Cuando el usuario ingresa un servidor o puerto erróneo, se muestra el siguiente resultado por pantalla:
<p align="center">
<img src="https://i.ibb.co/w7LhQQt/Sin-t-tulo.png" alt="drawing"  />
</p>

## Tecnologías utilizadas

* [C](https://devdocs.io/c/) - General-purpose, procedural computer programming language supporting structured programming,

## Autores

* **Artola Bianca** - [BiancaArtola](https://github.com/BiancaArtola)
* **Agustina Superi** - [AgusSuperi](https://github.com/AgusSuperi)

