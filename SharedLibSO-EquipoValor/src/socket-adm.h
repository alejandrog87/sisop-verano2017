/*
 * socket-adm.h
 *
 *  Created on: 11/9/2016
 *      Author: utnso
 */

#ifndef SRC_SOCKETS_SOCKET_ADM_H_
#define SRC_SOCKETS_SOCKET_ADM_H_
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <commons/log.h>
#include <stddef.h>
#include <pthread.h>
#define TIPO_SERVIDOR 0
#define TIPO_CLIENTE 1
#define PROCESO "socket-adm"
/**
 * @NAME: socket_create
 * @DESC: Genera un socket con un comportamiento servidor o cliente.
 * Para el caso servidor devuelve el socket generado en escucha (despues de listen).
 * Para el caso cliente devuelve el socket generado conectado (despues de connect).
 * Devuelve el descriptor/socket generado. En caso de error retorna 0.
 * @PARAMS:
 * ip -> a conectarse/donde escuchar
 * puerto -> a conectarse/donde escuchar
 * tipo_socket -> cliente o servidor
 * cant_clientes -> cantidad de cliente que pueden encolarse para conectarse al servidor
 * logger -> instancia del logger de la libreria commmons
 */
int socket_create(char* ip,char* puerto,char tipo_socket, int cant_clientes, t_log* logger);
/**
 * @NAME: send_all
 * @DESC: Envia la cantidad de bytes especificados por buffer_size.
 * Devuelve 0 en caso de error.
 */
int sendall(int socket_enviar, char *buf, int buffer_size);
/**
 * @NAME: recv_all
 * @DESC: Recibe la cantidad de bytes especificados por buffer_size.
 * Devuelve 0 en caso de error.
 */
int recvall(int socket_recibir, char *buffer, int buffer_size);
/**
 * @NAME: socket_accept
 * @DESC: Dado un socket/descriptor, acepta una conexión por el mismo y devuelve el socket/descriptor del cliente que se acaba de conectar.
 *En caso de error devuelve 0.
 */
int socket_accept(int socket_servidor);
/**
 * @NAME: socket_close
 * @DESC: Cierra un socket pasado por parametro. En caso de error devuelve 0.
 */
int socket_close(int socket_a_cerrar);

/**
 * @NAME: socket_thread_per_connection
 * @DESC: Genera un socket del tipo servidor,
 * escucha conexiones por el mismo y por cada conexion genera un hilo DETACHABLE (no se espera que termine)
 * que ejecuta una función pasada por parametro.
 * @PARAMS:
 * ip -> ip de escucha
 * puerto -> puerto de escucha
 * server_queue -> cantidad de conexiones que se pueden encolar
 * connection_handler -> funcion que va a ejecutar el hilo,
 * debe respetar la siguiente sintaxis: void *connection_handler(void *)
 * recibe como parametro el socket/descriptor de la nueva conexion
 * logger -> instancia de logger de la libreria de commons
 * thread_params -> parametros que debe conocer el nuevo hilo, una estructura por lo general
 */
int socket_thread_per_connection(char *ip, char* puerto, int server_queue,void (*connection_handler), t_log *logger);
/**
 * @NAME: socket_multiplexing
 * @DESC: Genera un socket del tipo servidor y escucha conexiones por el mismo, luego realiza multiplexacion
 * para:  conexiones nuevas, descriptores listos para leer y descriptores listos para escribir, para cada tipo realiza una funcion
 * @PARAMS:
 * ip -> ip de escucha
 * puerto -> puerto de escucha
 * server_queue -> cantidad de conexiones que se pueden encolar
 * master -> set de descriptores maestro
 * read_fds -> set de descriptores de lectura
 * write_fds -> set de descriptore de escritura
 * fdmax -> entero cuyo valor es el maximo descriptor
 * operacion_nuevas_conexiones -> funcion a ejecutar para nuevas conexiones, descriptor listener,
 * debe ser del tipo void *operacion_nuevas_conexiones(void *) recibe el descriptor de la nueva conexion
 * operacion_conexiones_leer -> funcion a ejecutar para los descriptores listos para leer read_fd
 * debe ser del tipo void *operacion_conexiones_leer(void *) recibe el descriptor listo para leer
 * operacion_conexiones_escribir -> funcion a ejecutar para los decriptores listor para escribir write_fd
 * debe ser del tipo void *operacion_conexiones_escribir(void *) recibe el descriptor listo para escribir
 * logger -> instancia de logger de la libreria commons
 */
int socket_multiplexing( char *ip, char* puerto, int server_queue, void (*operacion_nuevas_conexiones)(int), int (*operacion_conexiones_leer)(int), void (*operacion_conexiones_escribir)(int),t_log *logger);
#endif /* SRC_SOCKETS_SOCKET_ADM_H_ */
