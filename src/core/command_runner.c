#include "../../headers/core/command_runner.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Initialise le lanceur avec le chemin, les arguments et l'environnement.
void command_runner_init(CommandRunner *runner, char *path, char **args, char **env) {
    runner->path = path;
    runner->args = args;
    runner->env = env;
}

// Exécute la commande dans le processus enfant.
static void command_runner_exec_child(CommandRunner *runner) {
    execve(runner->path, runner->args, runner->env);
    perror("execve");
    _exit(127);
}

// Attend la fin du processus enfant et conserve son statut.
static CommandRunnerStatus command_runner_wait_child(pid_t pid, ProcessResult *result) {
    int status;

    while (waitpid(pid, &status, 0) < 0) {
        if (errno != EINTR) {
            return COMMAND_RUNNER_WAIT_ERROR;
        }
    }

    process_result_init(result, status);
    return COMMAND_RUNNER_SUCCESS;
}

// Lance la commande et remplit le résultat du processus enfant.
CommandRunnerStatus command_runner_run(CommandRunner *runner, ProcessResult *result) {
    pid_t pid = fork();

    if (pid < 0) {
        return COMMAND_RUNNER_FORK_ERROR;
    } else if (pid == 0) {
        command_runner_exec_child(runner);
    }

    return command_runner_wait_child(pid, result);
}
