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

typedef struct {
    int signal;
    void (*signal_handler)(int);
    t_log *logger;
} monitor_signal_struct;

/**
 * @NAME: create_signal_monitor
 * @DESC: Genera un hilo para tratar la señal <signal> pasada por parametro, la función que se utilizará para
 * tratar la señal es la que se pasa por parametro <signal_handler>. En caso de no poder generar el hilo retorna 0.
 * @PARAMS:
 * signal -> señal a tratar
 * signal_handler -> función que tratará la señal pasada por parámetro
 * logger -> instancia del logger de la libreria commmons
 */
int create_thread_signal_monitor(int signal,void (*signal_handler)(int),t_log *logger);

#endif /* SRC_MONITORS_ADM_H_ */
