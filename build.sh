#!/bin/bash

# Run pkg-config with the modules in the BUILD_PKG_CONFIG_MODULES environment variable, if this 
# variable exists.
function pkg_config_flags {
    if [[ -n "$BUILD_PKG_CONFIG_MODULES" ]]; then
        pkg-config --libs --cflags $BUILD_PKG_CONFIG_MODULES
    fi
}

function generate_clangd_compile_commands {
    local cmd="$1"
    local build_dir="$2"
    local build_file="$3"

    # Here we escape " and \ in $cmd
    local cmd_escaped="$(sed 's/\(["\]\)/\\\1/g' <<< "$cmd")"

    # heredoc so no indentation
cat >"$build_dir/compile_commands.json" <<- EOF
[
    {
        "directory": "$PWD", "file": "$build_file",
        "command": "$cmd_escaped"
    }
]
EOF

}

function build_linux_macos()
{
  echo "Building for Linux / MacOS target..."
  mkdir -p bin
  
  build_cmd="g++ $1.cpp -o ./bin/$1 -std=c++2a -O2 $(pkg_config_flags) $2"
  
  # Generate compile database before building, so that clangd can also highlight build errors.
  generate_clangd_compile_commands "$build_cmd" bin "$1.cpp"

  echo $build_cmd
  $build_cmd

  echo "Done."
}

function show_help() {
    echo "Usage: ./<build_script> [output_name] [option]"
    echo "Options:"
    echo "  help         : Shows help explaining command line arguments"
    echo "  h            : Same as 'help'"
}

# Check the number of arguments provided
if [ "$#" -lt 1 ]; then
    echo "Insufficient arguments. Usage: ./<build_script> [output_name] [option]"
    exit 1
fi

option=${2:-}
optional_flags=${3:-}

case "$option" in
    'help' | 'h')
        show_help
        ;;
    *)
        #echo "Invalid option. Use './<build_script> help' to see available options."
        build_linux_macos "$1" "$optional_flags"
        ;;
esac
