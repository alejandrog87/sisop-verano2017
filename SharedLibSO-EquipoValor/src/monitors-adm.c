/*
 * monitors-adm.c
 *
 *  Created on: 20/2/2017
 *      Author: utnso
 */

#include "monitors-adm.h"

void *create_signal_monitor(void* struct_m){

	monitor_signal_struct *st_monitor = struct_m;
	struct sigaction susr;
	susr.sa_handler = st_monitor->signal_handler; //un struct para cada signal a tratar, struct del sigusr1
	susr.sa_flags = SA_RESTART; // si no se coloca este flag, la system call se interrumpe
	sigemptyset(&susr.sa_mask); //sa_mask contiene los signals que se pueden bloquear mientras, se trata el signal actual

	if (sigaction(st_monitor->signal, &susr, NULL) == -1) {
		log_error(st_monitor->logger,
				"create_signal_monitor -> error al catchear signal ");
		return NULL;
	}

	while (1)
		sleep(1);
	return NULL;
}

int create_thread_signal_monitor(int signal,void (*signal_handler)(int),t_log *logger){

	pthread_attr_t attr;
	pthread_t thread_signal;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	void *res;
	monitor_signal_struct st_signal_monitor;
	st_signal_monitor.logger=logger;
	st_signal_monitor.signal_handler=signal_handler;
	st_signal_monitor.signal=signal;


	if (pthread_create(&thread_signal, &attr, &create_signal_monitor, (void*)&st_signal_monitor) < 0) {
		log_error(logger,
				"monitors-adm->create_thread_signal_monitor -> error al crear el hilo para atender signal");
	}
	if(!pthread_attr_destroy(&attr)){
		log_trace(logger,
					"monitors-adm->create_thread_signal_monitor -> hilo para atender signal creado");
	}else{
		log_error(logger,
							"monitors-adm->create_thread_signal_monitor ->fallo el pthread destroy");
	}

	if(!pthread_join(thread_signal, &res)){
		log_trace(logger,
						"monitors-adm->create_thread_signal_monitor -> hilo joineado");
	}else{
		log_error(logger,
						"monitors-adm->create_thread_signal_monitor ->fallo el pthread join");
	}
	free(res);
	return 1;

}
