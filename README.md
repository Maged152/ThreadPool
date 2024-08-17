# ThreadPool


# Build & Targets

## Configure 
    $ cmake -S <source_dir> -B <build_dir>

You can use `presets`

    $ cmake -S <source_dir> --preset <preset_name>

To know the existing presets

    $ cmake -S <source_dir> --list-presets


## Build
    $ cmake --build <build_dir>

## Install
    $ cmake --install <build_dir> --prefix <install_dir>

## Generate Documentations
    $ cmake --build <build_dir> --target documentation

## Examples
    $ cmake --build <build_dir> --target Example_<example_name>

## Benchmark
    $ cmake --build <build_dir> --target Benchmark_<benchmark_name>
