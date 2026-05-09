#!/bin/sh

set -u

echecs=0

lancer_test() {
    nom="$1"
    code_attendu="$2"
    commande="$3"
    sortie_attendue="$4"

    sortie="$(sh -c "$commande" 2>&1)"
    code="$?"

    if [ "$code" -ne "$code_attendu" ]; then
        echo "ECHEC: $nom - code attendu $code_attendu, obtenu $code"
        echo "$sortie"
        echecs=$((echecs + 1))
        return
    fi

    if ! printf '%s\n' "$sortie" | grep -F "$sortie_attendue" >/dev/null 2>&1; then
        echo "ECHEC: $nom - sortie manquante: $sortie_attendue"
        echo "$sortie"
        echecs=$((echecs + 1))
        return
    fi

    echo "SUCCES: $nom"
}

lancer_test "sans argument" 1 "./bin/watcher" "Usage:"
lancer_test "aide anglaise" 0 "./bin/watcher -h" "Watcher - User Manual"
lancer_test "aide francaise" 0 "./bin/watcher -hfr" "Watcher - Manuel d'utilisation"
lancer_test "commande en succes" 0 "./bin/watcher /bin/true" "Child exited with status 0"
lancer_test "commande en echec" 1 "./bin/watcher /bin/false" "Child exited with status 1"
lancer_test "code de sortie specifique" 42 "./bin/watcher /bin/sh -c 'exit 42'" "Child exited with status 42"
lancer_test "commande absente" 127 "./bin/watcher /no/such/command" "execve: No such file or directory"
lancer_test "terminaison par signal" 143 "./bin/watcher /bin/sh -c 'kill -TERM \$\$'" "Terminated by signal: 15"
lancer_test "transmission des arguments" 0 "./bin/watcher /bin/echo bonjour watcher test" "bonjour watcher test"
lancer_test "commande sans chemin" 127 "./bin/watcher true" "execve: No such file or directory"
lancer_test "terminaison par timeout" 143 "./bin/watcher --timeout 1 /bin/sleep 5" "Terminated by signal: 15"
lancer_test "timeout termine le groupe de processus" 143 "./bin/watcher --timeout 1 /bin/sh -c 'sleep 5 & wait'" "Terminated by signal: 15"
lancer_test "valeur timeout manquante" 1 "./bin/watcher --timeout" "Usage:"
lancer_test "valeur timeout invalide" 1 "./bin/watcher --timeout abc /bin/true" "Usage:"
lancer_test "valeur timeout zero" 1 "./bin/watcher --timeout 0 /bin/true" "Usage:"

if [ "$echecs" -ne 0 ]; then
    echo "$echecs test(s) en echec"
    exit 1
fi

echo "Tous les tests CLI sont passes"
