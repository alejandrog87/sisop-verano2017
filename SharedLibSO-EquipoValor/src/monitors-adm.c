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
		return 0;
	}
	if(!pthread_attr_destroy(&attr)){
		log_trace(logger,
				"monitors-adm->create_thread_signal_monitor -> hilo para atender signal creado");
	}else{
		log_error(logger,
				"monitors-adm->create_thread_signal_monitor ->fallo el pthread destroy");
		return 0;
	}

	if(!pthread_join(thread_signal, &res)){
		log_trace(logger,
				"monitors-adm->create_thread_signal_monitor -> hilo joineado");
	}else{
		log_error(logger,
				"monitors-adm->create_thread_signal_monitor ->fallo el pthread join");
		return 0;
	}
	free(res);
	return 1;

}

void *create_file_monitor(void* struct_m){
	monitor_file_struct  *st_monitor = struct_m;
	void (*func_handler)(void*);
	func_handler = st_monitor->file_handler;
	char buffer[BUF_LEN];

	log_trace(st_monitor->logger, "create_file_monitor -> init");

	// Al inicializar inotify este nos devuelve un descriptor de archivo
	int file_descriptor = inotify_init();
	if (file_descriptor < 0) {
		log_error(st_monitor->logger, "create_file_monitor -> Error al inicializar inotify");
		EXIT_FAILURE;

	}

	// Creamos un monitor sobre un path indicando que eventos queremos escuchar
	// El evento que me interesa a mi es el IN_CLOSE_WRITE, cuando se cierra un archivo
	// que se abrió para escribir. Por ejemplo un archivo de config.
	// Escuchamos sobre el directorio indicado en st_monitor->path
	int watch_descriptor = inotify_add_watch(file_descriptor, st_monitor->path, IN_CLOSE_WRITE);

	// El file descriptor creado por inotify, es el que recibe la información sobre los eventos ocurridos
	// para leer esta información el descriptor se lee como si fuera un archivo comun y corriente pero
	// la diferencia esta en que lo que leemos no es el contenido de un archivo sino la información
	// referente a los eventos ocurridos
	int length = read(file_descriptor, buffer, BUF_LEN);
	while(length >= 0){
		//if (length < 0) {
		//	log_error(st_monitor->logger, "create_file_monitor -> Error al leer el descriptor de inotify");
		//	EXIT_FAILURE;
		//}

		int offset = 0;

		// Luego del read buffer es un array de n posiciones donde cada posición contiene
		// un eventos ( inotify_event ) junto con el nombre de este.
		while (offset < length) {

			// El buffer es de tipo array de char, o array de bytes. Esto es porque como los
			// nombres pueden tener nombres mas cortos que 24 caracteres el tamaño va a ser menor
			// a sizeof( struct inotify_event ) + 24.
			struct inotify_event *event = (struct inotify_event *) &buffer[offset];

			// El campo "len" nos indica la longitud del tamaño del nombre
			if (event->len) {
				// Dentro de "mask" tenemos el evento que ocurrio y sobre donde ocurrio
				// sea un archivo o un directorio
				if (event->mask & IN_CLOSE_WRITE) {
					if (event->mask & IN_ISDIR) {
						log_trace(st_monitor->logger, "create_file_monitor -> El directorio %s fue modificado", event-> name);
					} else {
						//como escuchamos sobre un directorio verifico que el evento sea sobre el archivo que me interesa
						//indicado en st_monitor->fileName

						if(strcasecmp(event-> name, st_monitor->fileName) == 0){
							log_trace(st_monitor->logger, "create_file_monitor -> El archivo %s fue modificado ", event-> name);
							func_handler(struct_m);
						}
					}
				}
				/*ACA SE PUEDEN MONITOREAR OTROS EVENTOS Y AGREGAR OTROS HANDLERS*/
				/*} else if (event->mask & IN_DELETE) {
					if (event->mask & IN_ISDIR) {
						printf("The directory %s was deleted.\n", event->name);
					} else {
						printf("The file %s was deleted.\n", event->name);
					}
				} else if (event->mask & IN_MODIFY) {
					if (event->mask & IN_ISDIR) {
						printf("The directory %s was modified.\n", event->name);
					} else {
						printf("The file %s was modified.\n", event->name);
					}
				}*/
			}
			offset += sizeof (struct inotify_event) + event->len;
		}
		length = read(file_descriptor, buffer, BUF_LEN);
	}
	inotify_rm_watch(file_descriptor, watch_descriptor);
	close(file_descriptor);

	return EXIT_SUCCESS;

}

int create_thread_file_monitor(char* path,void (*handler)(void*),t_log *logger,char* fileName){
	pthread_attr_t attr;
	pthread_t thread_signal;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	void *res;
	monitor_file_struct st_file_monitor;
	st_file_monitor.logger=logger;
	st_file_monitor.file_handler=handler;
	st_file_monitor.path=path;
	st_file_monitor.fileName=fileName;


	if (pthread_create(&thread_signal, &attr, &create_file_monitor, (void*)&st_file_monitor) < 0) {
		log_error(logger,
				"monitors-adm->create_thread_file_monitor -> error al crear el hilo para atender file");
		return 0;
	}
	if(!pthread_attr_destroy(&attr)){
		log_trace(logger,
				"monitors-adm->create_thread_file_monitor -> hilo para atender file creado");
	}else{
		log_error(logger,
				"monitors-adm->create_thread_file_monitor ->fallo el pthread destroy");
		return 0;
	}

	if(!pthread_join(thread_signal, &res)){
		log_trace(logger,
				"monitors-adm->create_thread_file_monitor -> hilo joineado");
	}else{
		log_error(logger,
				"monitors-adm->create_thread_file_monitor ->fallo el pthread join");
		return 0;
	}
	free(res);
	return 1;
}
