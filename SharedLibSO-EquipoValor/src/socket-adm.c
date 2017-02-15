#include "socket-adm.h"


int socket_create(char* ip,char* puerto,char tipo_socket, int cant_clientes,t_log* logger){
	struct addrinfo hints, *servinfo, *p;
	int rv,socket_creado;
	int yes=1;
	//char s[INET6_ADDRSTRLEN];
	log_trace(logger,"socket_create -> init");
	memset(&hints, 0, sizeof hints); //inicializar estructura
	hints.ai_family = AF_UNSPEC; //AF_INET or AF_INET6
	hints.ai_socktype = SOCK_STREAM; //TIPO SOCKET
	hints.ai_flags = AI_PASSIVE; // PARA USAR LA IP LOCALHOST, DPS SE IGNORA
	//TENGO una lista linkeada en servinfo ,primer argumento ip especifica ignora flag
	if ((rv = getaddrinfo(ip, puerto, &hints, &servinfo)) != 0) {
		return 0;
	}
	log_trace(logger,"getaddrinfo -> complete");
	//creo socket con un elemento valido de la estructura de getaddrinfo
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_creado = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			log_trace(logger,"socket call -> error");
			continue;
		}
		log_trace(logger,"socket -> complete");
		if(tipo_socket == TIPO_SERVIDOR){
			log_trace(logger,"socket TIPO_SERVIDOR");
			if (setsockopt(socket_creado, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
				log_error(logger,"setsockopt -> error");
				freeaddrinfo(servinfo);
				return 0;
			}
			if (bind(socket_creado, p->ai_addr, p->ai_addrlen) == -1) {//bindeo puerto e ip de la estructura esa
				close(socket_creado);
				log_trace(logger,"bind -> error");
				continue;
			}
		}else{
			log_trace(logger,"socket TIPO_CLIENTE");
			if (connect(socket_creado, p->ai_addr, p->ai_addrlen) == -1) {
				close(socket_creado);
				continue;
			}
		}
		break;
	}
	if (p == NULL) {
		log_error(logger,"bind -> error");
		freeaddrinfo(servinfo);
		return 0;
	}
	if(tipo_socket == TIPO_SERVIDOR){
		if (listen(socket_creado, cant_clientes) == -1) {
			log_error(logger,"listen -> error");
			freeaddrinfo(servinfo);
			return 0;
		}
	}
	freeaddrinfo(servinfo); // libero estructura getaddrinfo
	return socket_creado;
}


int sendall(int socket_enviar, char *buffer, int buffer_size){
	int total = 0; // cuanto bytes enviamos
	int bytesleft = buffer_size; // cuantos bytes faltan enviar
	int n;
	while(total < buffer_size) {
		n = send(socket_enviar, buffer+total, bytesleft, 0);
		if (n == -1)return 0;
		total += n;
		bytesleft -= n;
	}
	return 1;
}


int recvall(int socket_recibir, char *buffer, int buffer_size){
	int nRet;
	while((nRet = recv(socket_recibir, buffer, buffer_size, 0))>0){
		buffer_size=buffer_size - nRet;
		if(buffer_size==0){
			return 1;
		}
	}
	if(nRet==-1){
		return -1;
	}else{
		if(nRet==0){
			return 0;
		}
	}
		return 1;
}


int socket_accept(int socket_servidor){
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	int socket_cliente;
	sin_size = sizeof their_addr;
	socket_cliente = accept(socket_servidor, (struct sockaddr *)&their_addr, &sin_size);
	if (socket_cliente == -1) {
		return 0;
	}
	return socket_cliente;
}


int socket_close(int socket_a_cerrar){
	if(close(socket_a_cerrar) == -1){
		return 0;
	}
	return 1;
}


int socket_thread_per_connection(char *ip, char* puerto, int server_queue,void (*connection_handler), t_log *logger){
	int socket_cliente;
	int *sock_thread;
	int socket_serv = socket_create(ip,puerto,TIPO_SERVIDOR, server_queue, logger);
	if(socket_serv !=0){
		log_trace(logger,"socket_thread_per_connection -> socket servidor creado");
	}else{
		log_error(logger,"socket_thread_per_connection -> no se pudo crear el socket servidor");
		return 0;
	}
	while( (socket_cliente = socket_accept(socket_serv)) != 0)
	{
		log_trace(logger,"socket_thread_per_connection -> conexion aceptada");
		pthread_attr_t attr;
		pthread_t sniffer_thread;
		sock_thread = malloc(sizeof(int));
		*sock_thread = socket_cliente;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		if( pthread_create( &sniffer_thread , &attr , connection_handler , (void*)sock_thread) < 0)
		{
			log_error(logger,"socket_thread_per_connection -> error al crear el hilo");
			return 0;
		}
		pthread_attr_destroy(&attr);
		log_trace(logger,"socket_thread_per_connection -> handler asignado");
	}
	if (socket_cliente == 0)
	{
		log_error(logger,"socket_thread_per_connection -> fallo el accept");
		return 0;
	}
	return 1;
}

int socket_multiplexing( char *ip, char* puerto, int server_queue, void (*operacion_nuevas_conexiones)(int), int (*operacion_conexiones_leer)(int), void (*operacion_conexiones_escribir)(int),t_log *logger){
	int i,new_fd;
	fd_set master;
	fd_set read_fds;
	fd_set write_fds;
	int fdmax;
	int socket_serv = socket_create(ip,puerto,TIPO_SERVIDOR, server_queue, logger);

	if(socket_serv !=0){
		log_trace(logger,"socket_multiplexing -> socket servidor creado");
	}else{
		log_error(logger,"socket_multiplexing -> no se pudo crear el socket servidor");
		return 0;
	}

	//Agrego al listener a la lista de descriptores master, lo voy a neesitar para el select
	FD_SET(socket_serv, &master);

	//El maximo descriptor hasta ahora es el listener, necesito el maximo para despues loopear
	fdmax = socket_serv;

	log_trace(logger,"Esperando conexiones entrantes...");

	// loopeando en el set de fd de lectura
	for(;;) {
		read_fds = master; // copio master a los fd de lectura

		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			log_error(logger,"Error al ejecutar el select");
			exit(4);
		}

		// buscando datos para leer
		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // se lee este
				if (i == socket_serv) {
					// nueva conexion
					if((new_fd = socket_accept(socket_serv)) == 0){
						log_error(logger,"Error al aceptar nueva conexion");
					} else {
						FD_SET(new_fd, &master); // se agrega al master
						if (new_fd > fdmax) {    // siempre actualizar el maximo para el loop
							fdmax = new_fd;
						}
						log_trace(logger,"Nueva conexion");
						(*operacion_nuevas_conexiones)(new_fd);
						//EJECUTAR FUNCION PARA NUEVAS CONEXIONES
					}
				} else {
					if((*operacion_conexiones_leer)(i)==0){
						FD_CLR(i, &master);//para clientes desconectados
					}
					//EJECUTAR FUNCION PARA DESCRIPTORES LECTURA READ_FD

				}
			}else{
				if (FD_ISSET(i, &write_fds)) {
					(*operacion_conexiones_escribir)(i);
					//EJECUTAR FUNCION PARA DESCRIPTORES ESCRITURAs READ_FD
				}
			}
		} //Fin loop de descriptores
	}

	return 1;
}
