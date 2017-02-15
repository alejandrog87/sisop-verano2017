/*
 * test-socket-client.c
 *
 *  Created on: 11/9/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include "src/socket-adm.h"
#include <string.h>


#define IP_CONECTAR "127.0.0.1"
#define PUERTO_CONECTAR "5002"
#define PROCESO_C "procesoCliente"
#define RUTA_LOG_C "log/Cliente.log"
#define MAX_LONG_PAQUETE 57

int main(void) {
	//creo instancia de logger
	t_log* logger = log_create(RUTA_LOG_C, PROCESO_C, true, LOG_LEVEL_TRACE);
	int *buffer_size=malloc(sizeof(int));
	//char buffer[MAX_LONG_PAQUETE];
	int socket_cliente = socket_create(IP_CONECTAR,PUERTO_CONECTAR,TIPO_CLIENTE, 0, logger);
	if(socket_cliente !=0){
		log_trace(logger,"Main -> socket cliente creado");
		printf("Cliente conectado");
	}else{
		exit(0);
	}


	close(socket_cliente);

	free(buffer_size);
	return EXIT_SUCCESS;
}


