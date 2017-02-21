/*
 * test-signals.c
 *
 *  Created on: 20/2/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include "src/monitors-adm.h"

#define PROCESO_S "procesoSignals"

#define RUTA_LOG_S "log/Signal.log"
t_log* logger;


void custom_handler (int i_signal){

	if (i_signal == SIGUSR1)
		printf("received SIGUSR1\n");
	else if (i_signal == SIGKILL)
		printf("received SIGKILL\n");
	else if (i_signal == SIGTERM)
		printf("received SIGTERM\n");

}

int main(void) {
	logger = log_create(RUTA_LOG_S, PROCESO_S, true, LOG_LEVEL_TRACE);

	if(!create_thread_signal_monitor(SIGUSR1,&custom_handler,logger)){
		log_error(logger,"main-> La función create_thread_signal_monitor devolvio error");
		//return 0;
	}

	return 1;

}


