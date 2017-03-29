/*
 * monitors-adm.h
 *
 *  Created on: 20/2/2017
 *      Author: utnso
 */

#ifndef SRC_MONITORS_ADM_H_
#define SRC_MONITORS_ADM_H_

#include <stddef.h>
#include <commons/log.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>

typedef struct {
    int signal;
    void (*signal_handler)(int);
    t_log *logger;
} monitor_signal_struct;

typedef struct {
    char *path;
    void (*file_handler)(void*);
    t_log *logger;
    char *fileName;
} monitor_file_struct;


// El tamaño de un evento es igual al tamaño de la estructura de inotify
// mas el tamaño maximo de nombre de archivo que nosotros soportemos
// en este caso el tamaño de nombre maximo que vamos a manejar es de 24
// caracteres. Esto es porque la estructura inotify_event tiene un array
// sin dimension ( Ver C-Talks I - ANSI C ).
#define EVENT_SIZE  ( sizeof (struct inotify_event) + 24 )

// El tamaño del buffer es igual a la cantidad maxima de eventos simultaneos
// que quiero manejar por el tamaño de cada uno de los eventos. En este caso
// Puedo manejar hasta 1024 eventos simultaneos.
#define BUF_LEN     ( 1024 * EVENT_SIZE )

/**
 * @NAME: create_thread_signal_monitor
 * @DESC: Genera un hilo para tratar la señal <signal> pasada por parametro, la función que se utilizará para
 * tratar la señal es la que se pasa por parametro <signal_handler>. En caso de no poder generar el hilo retorna 0.
 * @PARAMS:
 * signal -> señal a tratar
 * signal_handler -> función que tratará la señal pasada por parámetro, lo que se tiene que hacer cuando se reciba la señal.
 * logger -> instancia del logger de la libreria commmons
 */
int create_thread_signal_monitor(int signal,void (*signal_handler)(int),t_log *logger);


/**
 * @NAME: create_path_file_monitor
 * @DESC: Genera un hilo monitor para un determinado path de un archivo, en caso de modificarse el archivo dentro del path
 * ejecuta la función <handler> pasada por parametro.En caso de error devuelve 0.
 * @PARAMS:
 * path -> string que representa la ruta a monitorear
 * signal_handler -> función que ejecutara si el evento ocurre (modificación de archivo).
 * logger -> instancia del logger de la libreria commmons
 */
int create_thread_file_monitor(char* path,void (*handler)(void*),t_log *logger,char* fileName);


#endif /* SRC_MONITORS_ADM_H_ */
