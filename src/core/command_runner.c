#define _POSIX_C_SOURCE 200809L

#include "core/command_runner.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Initialise le lanceur avec le chemin, les arguments et l'environnement.
void command_runner_init(CommandRunner *runner, char *path, char **args, char **env, int timeout_seconds) {
    if (runner == NULL) {
        return;
    }

    runner->path = path;
    runner->args = args;
    runner->env = env;
    runner->timeout_seconds = timeout_seconds;
}

// Exécute la commande dans le processus enfant.
static void command_runner_exec_child(CommandRunner *runner) {
    if (setpgid(0, 0) < 0) {
        perror("setpgid");
        _exit(127);
    }

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

// Attend l'enfant avec une limite de temps, puis le termine si nécessaire.
static CommandRunnerStatus command_runner_wait_child_with_timeout(
    pid_t pid,
    ProcessResult *result,
    int timeout_seconds
) {
    int status;
    int elapsed_seconds = 0;
    int grace_seconds = 0;

    while (elapsed_seconds < timeout_seconds) {
        pid_t wait_result = waitpid(pid, &status, WNOHANG);

        if (wait_result == pid) {
            process_result_init(result, status);
            return COMMAND_RUNNER_SUCCESS;
        }

        if (wait_result < 0 && errno == EINTR) {
            sleep(1);
            elapsed_seconds++;
            continue;
        }

        if (wait_result < 0) {
            return COMMAND_RUNNER_WAIT_ERROR;
        }

        if (wait_result == 0) {
            sleep(1);
            elapsed_seconds++;
        }
    }

    if (kill(-pid, SIGTERM) < 0 && errno != ESRCH) {
        return COMMAND_RUNNER_WAIT_ERROR;
    }

    while (grace_seconds < 1) {
        pid_t wait_result = waitpid(pid, &status, WNOHANG);

        if (wait_result == pid) {
            process_result_init(result, status);
            return COMMAND_RUNNER_SUCCESS;
        }

        if (wait_result < 0 && errno == EINTR) {
            sleep(1);
            grace_seconds++;
            continue;
        }

        if (wait_result < 0) {
            return COMMAND_RUNNER_WAIT_ERROR;
        }

        if (wait_result == 0) {
            sleep(1);
            grace_seconds++;
        }
    }

    if (kill(-pid, SIGKILL) < 0 && errno != ESRCH) {
        return COMMAND_RUNNER_WAIT_ERROR;
    }

    return command_runner_wait_child(pid, result);
}

// Lance la commande et remplit le résultat du processus enfant.
CommandRunnerStatus command_runner_run(CommandRunner *runner, ProcessResult *result) {
    pid_t pid;

    if (runner == NULL || result == NULL || runner->path == NULL || runner->args == NULL) {
        return COMMAND_RUNNER_INVALID_ARGUMENT;
    }

    pid = fork();

    if (pid < 0) {
        return COMMAND_RUNNER_FORK_ERROR;
    } else if (pid == 0) {
        command_runner_exec_child(runner);
    }

    if (runner->timeout_seconds > 0) {
        if (setpgid(pid, pid) < 0 && errno != EACCES) {
            return COMMAND_RUNNER_WAIT_ERROR;
        }

        return command_runner_wait_child_with_timeout(pid, result, runner->timeout_seconds);
    }

    return command_runner_wait_child(pid, result);
}
