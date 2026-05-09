#include "../../headers/ui/console_view.h"

#include <stdio.h>

// Affiche l'utilisation attendue du programme.
void console_view_print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s <command> [args...]\n", program_name);
}

// Affiche le manuel d'utilisation complet.
void console_view_print_manual(void) {
    printf("Watcher - Manuel d'utilisation\n");
    printf("\n");
    printf("Usage:\n");
    printf("  watcher <commande> [arguments...]\n");
    printf("  watcher -h\n");
    printf("\n");
    printf("Description:\n");
    printf("  Watcher lance une commande dans un processus enfant, attend sa fin,\n");
    printf("  puis affiche son code de sortie et le signal de terminaison éventuel.\n");
    printf("\n");
    printf("Arguments:\n");
    printf("  <commande>      Chemin de l'exécutable à lancer, par exemple /bin/ls.\n");
    printf("  [arguments...]  Arguments transmis à la commande.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -h              Affiche ce manuel d'utilisation.\n");
    printf("\n");
    printf("Exemples:\n");
    printf("  watcher /bin/ls -la\n");
    printf("  watcher /usr/bin/env\n");
}

// Affiche l'erreur produite par le lanceur de commande.
void console_view_print_command_runner_error(CommandRunnerStatus status) {
    if (status == COMMAND_RUNNER_FORK_ERROR) {
        fprintf(stderr, "Erreur: impossible de créer le processus enfant\n");
    } else if (status == COMMAND_RUNNER_WAIT_ERROR) {
        fprintf(stderr, "Erreur: impossible d'attendre la fin du processus enfant\n");
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
