#!/bin/bash

# Setze das Arbeitsverzeichnis auf das Datenverzeichnis
cd "$(dirname "$0")"

# Exportiere LD_LIBRARY_PATH, um die mitgelieferten Bibliotheken zu finden
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:+$LD_LIBRARY_PATH:}$(dirname "$0")/../lib/${ARCH_TRIPLET}"

# Führe das Spiel aus
./boswars

exit 0
