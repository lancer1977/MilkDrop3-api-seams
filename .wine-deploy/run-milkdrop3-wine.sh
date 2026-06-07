#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"
export WINEPREFIX="${WINEPREFIX:-$REPO_ROOT/.wine-test}"
# Do not force WINEARCH here: the repo-local .wine-test prefix is already initialized
# as win64 with WoW64 support for this 32-bit executable.
export WINEDLLOVERRIDES="${WINEDLLOVERRIDES:-d3dx9_43=n,b;d3dcompiler_43=n,b}"
# Wine currently reports AUDCLNT_E_WRONG_ENDPOINT_TYPE for the app's WASAPI
# loopback capture path on this Linux host. This lets the patched build bring up
# the renderer without audio capture so the Wine/D3D path can be validated.
export MILKDROP_ALLOW_AUDIOLESS="${MILKDROP_ALLOW_AUDIOLESS:-1}"
EXE="$SCRIPT_DIR/MilkDrop3/MilkDrop 3.exe"
cd "$(dirname "$EXE")"
exec wine "$EXE" "$@"
