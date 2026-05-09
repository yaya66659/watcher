#ifndef WATCHER_20260509_102004_CEST_CORE_PROCESS_RESULT_H
#define WATCHER_20260509_102004_CEST_CORE_PROCESS_RESULT_H

typedef struct {
    int raw_status;
} ProcessResult;

// Initialise un résultat de processus.
void process_result_init(ProcessResult *result, int raw_status);

// Indique si le processus s'est terminé normalement.
int process_result_exited(const ProcessResult *result);

// Récupère le code de sortie uniquement s'il est disponible.
int process_result_get_exit_status(const ProcessResult *result, int *exit_status);

// Indique si le processus a été terminé par un signal.
int process_result_signaled(const ProcessResult *result);

// Récupère le signal uniquement s'il est disponible.
int process_result_get_signal(const ProcessResult *result, int *signal);

#endif
