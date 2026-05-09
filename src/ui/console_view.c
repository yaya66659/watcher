#include "ui/console_view.h"

#include <stdio.h>

// Affiche l'utilisation attendue du programme.
void console_view_print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [--timeout seconds] <executable_path> [args...]\n", program_name);
}

// Affiche le manuel d'utilisation complet.
void console_view_print_manual(void) {
    printf("Watcher - User Manual\n");
    printf("\n");
    printf("Usage:\n");
    printf("  watcher [--timeout seconds] <executable_path> [args...]\n");
    printf("  watcher -h\n");
    printf("  watcher -hfr\n");
    printf("\n");
    printf("Description:\n");
    printf("  Watcher starts a command in a child process, waits for it to finish,\n");
    printf("  then prints its exit status and termination signal if any.\n");
    printf("\n");
    printf("Arguments:\n");
    printf("  <executable_path>  Path to the executable to run, for example /bin/ls.\n");
    printf("  [args...]          Arguments passed to the command.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -h              Show this user manual.\n");
    printf("  -hfr            Show the French user manual.\n");
    printf("  --timeout N     Terminate the command if it exceeds N seconds.\n");
    printf("\n");
    printf("Examples:\n");
    printf("  watcher /bin/ls -la\n");
    printf("  watcher /usr/bin/env\n");
}

// Affiche le manuel d'utilisation complet en français.
void console_view_print_manual_fr(void) {
    printf("Watcher - Manuel d'utilisation\n");
    printf("\n");
    printf("Utilisation:\n");
    printf("  watcher [--timeout secondes] <chemin_executable> [arguments...]\n");
    printf("  watcher -h\n");
    printf("  watcher -hfr\n");
    printf("\n");
    printf("Description:\n");
    printf("  Watcher lance une commande dans un processus enfant, attend sa fin,\n");
    printf("  puis affiche son code de sortie et le signal de terminaison éventuel.\n");
    printf("\n");
    printf("Arguments:\n");
    printf("  <chemin_executable>  Chemin de l'exécutable à lancer, par exemple /bin/ls.\n");
    printf("  [arguments...]       Arguments transmis à la commande.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -h              Affiche le manuel d'utilisation en anglais.\n");
    printf("  -hfr            Affiche ce manuel d'utilisation en français.\n");
    printf("  --timeout N     Termine la commande si elle dépasse N secondes.\n");
    printf("\n");
    printf("Exemples:\n");
    printf("  watcher /bin/ls -la\n");
    printf("  watcher /usr/bin/env\n");
}

// Affiche l'erreur produite par le lanceur de commande.
void console_view_print_command_runner_error(CommandRunnerStatus status) {
    if (status == COMMAND_RUNNER_INVALID_ARGUMENT) {
        fprintf(stderr, "Error: invalid arguments for command execution\n");
    } else if (status == COMMAND_RUNNER_FORK_ERROR) {
        fprintf(stderr, "Error: unable to create child process\n");
    } else if (status == COMMAND_RUNNER_WAIT_ERROR) {
        fprintf(stderr, "Error: unable to wait for child process\n");
    }
}

// Affiche la façon dont le processus enfant s'est terminé.
void console_view_print_process_result(const ProcessResult *result) {
    int exit_status;
    int signal;

    if (process_result_get_exit_status(result, &exit_status)) {
        printf("Child exited with status %d\n", exit_status);
    } else {
        printf("Child exited with status N/A\n");
    }

    if (process_result_get_signal(result, &signal)) {
        printf("Terminated by signal: %d\n", signal);
    } else {
        printf("Terminated by signal: N/A\n");
    }
}
