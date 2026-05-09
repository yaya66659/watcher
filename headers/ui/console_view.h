#ifndef WATCHER_20260509_102004_CEST_UI_CONSOLE_VIEW_H
#define WATCHER_20260509_102004_CEST_UI_CONSOLE_VIEW_H

#include "../core/command_runner.h"
#include "../core/process_result.h"

// Affiche l'utilisation attendue du programme.
void console_view_print_usage(const char *program_name);

// Affiche le manuel d'utilisation complet.
void console_view_print_manual(void);

// Affiche une erreur liée au lanceur de commande.
void console_view_print_command_runner_error(CommandRunnerStatus status);

// Affiche le résultat d'exécution du processus.
void console_view_print_process_result(const ProcessResult *result);

#endif
