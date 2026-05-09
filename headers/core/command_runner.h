#ifndef WATCHER_20260509_102004_CEST_CORE_COMMAND_RUNNER_H
#define WATCHER_20260509_102004_CEST_CORE_COMMAND_RUNNER_H

#include "core/process_result.h"

typedef enum {
    COMMAND_RUNNER_SUCCESS = 0,
    COMMAND_RUNNER_INVALID_ARGUMENT,
    COMMAND_RUNNER_FORK_ERROR,
    COMMAND_RUNNER_WAIT_ERROR
} CommandRunnerStatus;

typedef struct {
    char *path;
    char **args;
    char **env;
    int timeout_seconds;
} CommandRunner;

// Initialise un lanceur de commande.
void command_runner_init(CommandRunner *runner, char *path, char **args, char **env, int timeout_seconds);

// Lance la commande et retourne son statut d'exécution.
CommandRunnerStatus command_runner_run(CommandRunner *runner, ProcessResult *result);

#endif
