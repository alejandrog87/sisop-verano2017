/*
 * test-files.c
 *
 *  Created on: 28/3/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include "src/monitors-adm.h"
#include <commons/config.h>

#define PROCESO_S "procesoFiles"

#define RUTA_LOG_S "log/Files.log"

#define RUTA_CONFIG_S "config/config.cfg"
t_log* logger;


typedef struct {
	int edad;
	char* nombre;
} Usuario ;

void imprimir_usuario(Usuario* aImprimir){
	log_debug(logger,"Valores actuales  de la estructura\n");
	log_debug(logger,"El usuario tiene nombre: %s\n",aImprimir->nombre);
	log_debug(logger,"El usuario tiene edad: %i\n",aImprimir->edad);
}

void custom_handler (void* argumentos){

	log_trace(logger,"custom_handler -> init");
	t_config *arch_cfg = config_create(RUTA_CONFIG_S);
	if(!arch_cfg){
		log_error(logger, "Error al crear el archivo de configuración arch_cfg\n");
	}else{
		Usuario usr;
		usr.edad = config_get_int_value(arch_cfg, "edad");
		usr.nombre= config_get_string_value(arch_cfg, "nombre");
		imprimir_usuario(&usr);
	}
	config_destroy(arch_cfg);
}

int main(void) {
	logger = log_create(RUTA_LOG_S, PROCESO_S, true, LOG_LEVEL_TRACE);

	if(!create_thread_file_monitor("config/",&custom_handler,logger,"config.cfg")){
		log_error(logger,"main-> La función create_thread_file_monitor devolvio error");
		//return 0;
	}

	return 1;

}


