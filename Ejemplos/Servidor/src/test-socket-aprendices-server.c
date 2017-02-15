/*
 ============================================================================
 Name        : test-socket-aprendices-server.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include "src/socket-adm.h"

#define IP_ESCUCHA "127.0.0.1"
#define PUERTO_ESCUCHA "5002"
#define COLA_SERVER 20

#define PROCESO_S "procesoServer"

#define RUTA_LOG_S "log/Server.log"

void operacion_nuevas_conexiones(int);
int operacion_conexiones_leer(int);
void operacion_conexiones_escribir(int);
t_log* logger;
t_queue* ready;

int main(void) {
	logger = log_create(RUTA_LOG_S, PROCESO_S, true, LOG_LEVEL_TRACE);
	ready = queue_create();
	//creo instancia de logger
	//t_log* logger = log_create(RUTA_LOG, PROCESO_S, true, LOG_LEVEL_TRACE);

	while(!socket_multiplexing( IP_ESCUCHA, PUERTO_ESCUCHA, COLA_SERVER, &(operacion_nuevas_conexiones), &(operacion_conexiones_leer), &(operacion_conexiones_escribir),logger));
	queue_destroy(ready);
	return EXIT_SUCCESS;
}

void operacion_nuevas_conexiones(int sock_fd){
	printf("Se recibió una nueva conexión\n");
}

int operacion_conexiones_leer(int sock_fd){
	return 1;
}

void operacion_conexiones_escribir(int sock_fd){
	return;
}
