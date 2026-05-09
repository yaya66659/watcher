#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "core/command_runner.h"
#include "ui/console_view.h"


extern char **environ;

static int parse_timeout_seconds(const char *value, int *timeout_seconds) {
    char *end;
    long parsed_value;

    errno = 0;
    parsed_value = strtol(value, &end, 10);

    if (errno != 0 || end == value || *end != '\0' || parsed_value <= 0 || parsed_value > INT_MAX) {
        return 0;
    }

    *timeout_seconds = (int)parsed_value;
    return 1;
}

// Point d'entrée du programme : valide les arguments et lance la commande.
int main(int argc, char *argv[]) {
    CommandRunner runner;
    ProcessResult result;
    CommandRunnerStatus status;
    int exit_status;
    int signal;
    int command_index = 1;
    int timeout_seconds = 0;

    if (argc < 2) {
        console_view_print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        console_view_print_manual();
        return EXIT_SUCCESS;
    }

    if (argc == 2 && strcmp(argv[1], "-hfr") == 0) {
        console_view_print_manual_fr();
        return EXIT_SUCCESS;
    }

    if (strcmp(argv[1], "--timeout") == 0) {
        if (argc < 4 || !parse_timeout_seconds(argv[2], &timeout_seconds)) {
            console_view_print_usage(argv[0]);
            return EXIT_FAILURE;
        }

        command_index = 3;
    }

    // Prépare la commande à partir des arguments reçus par le programme.
    command_runner_init(&runner, argv[command_index], &argv[command_index], environ, timeout_seconds);

    // Lance la commande et récupère le résultat du processus enfant.
    status = command_runner_run(&runner, &result);
    if (status != COMMAND_RUNNER_SUCCESS) {
        console_view_print_command_runner_error(status);
        return EXIT_FAILURE;
    }

    // Délègue l'affichage du résultat au module d'interface console.
    console_view_print_process_result(&result);

    if (process_result_get_exit_status(&result, &exit_status)) {
        return exit_status;
    }

    if (process_result_get_signal(&result, &signal)) {
        return 128 + signal;
    }

    return EXIT_FAILURE;
}
