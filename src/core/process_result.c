#include "core/process_result.h"

#include <sys/wait.h>

// Initialise le résultat avec le statut brut retourné par waitpid.
void process_result_init(ProcessResult *result, int raw_status) {
    result->raw_status = raw_status;
}

// Indique si le processus enfant s'est terminé normalement.
int process_result_exited(const ProcessResult *result) {
    return WIFEXITED(result->raw_status);
}

// Récupère le code de sortie uniquement s'il est disponible.
int process_result_get_exit_status(const ProcessResult *result, int *exit_status) {
    if (!process_result_exited(result)) {
        return 0;
    }

    *exit_status = WEXITSTATUS(result->raw_status);
    return 1;
}

// Indique si le processus enfant a été terminé par un signal.
int process_result_signaled(const ProcessResult *result) {
    return WIFSIGNALED(result->raw_status);
}

// Récupère le signal uniquement s'il est disponible.
int process_result_get_signal(const ProcessResult *result, int *signal) {
    if (!process_result_signaled(result)) {
        return 0;
    }

    *signal = WTERMSIG(result->raw_status);
    return 1;
}
