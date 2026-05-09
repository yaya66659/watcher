#include <stdlib.h>
#include <string.h>

#include "../headers/core/command_runner.h"
#include "../headers/ui/console_view.h"


extern char **environ;

// Point d'entrée du programme : valide les arguments et lance la commande.
int main(int argc, char *argv[]) {
    CommandRunner runner;
    ProcessResult result;
    CommandRunnerStatus status;
    int exit_status;
    int signal;

    if (argc < 2) {
        console_view_print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        console_view_print_manual();
        return EXIT_SUCCESS;
    }

    // Prépare la commande à partir des arguments reçus par le programme.
    command_runner_init(&runner, argv[1], &argv[1], environ);

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
