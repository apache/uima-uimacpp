#!/usr/bin/env bash
set -Eeo pipefail

# check to see if this file is being run or sourced from another script
_is_sourced() {
	# https://unix.stackexchange.com/a/215279
	[ "${#FUNCNAME[@]}" -ge 2 ] \
		&& [ "${FUNCNAME[0]}" = '_is_sourced' ] \
		&& [ "${FUNCNAME[1]}" = 'source' ]
}

_main() {
    # for generic AEs .so in /usr/local/uimacpp/ae
    
    # without LD_LIBRARY_PATH all annotators would need to be named 'lib' something
    # https://stackoverflow.com/q/11842729
    export LD_LIBRARY_PATH="/usr/local/uimacpp/ae"

    # for pythonnator, until a package is system installed
    export PYTHONPATH=/usr/local/uimacpp/scripts
    
    # for perltator, until the module is system installed
    export PATH=/usr/local/uimacpp/scripts:$PATH
    export PERLLIB=/usr/local/uimacpp/scripts:/usr/local/uimacpp/lib

    /usr/local/uimacpp/bin/runAECpp "$@"
}

if ! _is_sourced; then
	_main "$@"
fi
