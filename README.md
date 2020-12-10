# betterIR

betterIR is my personal research and development lab where I implement various 
compiler optimizations as out-of-tree LLVM passes. Currently, this repository
implements:
* Dead code elimination
* Liveness analysis (Testing in progress)

## Compile
To build this project, the following dependencies are required:
* CMake 3.0.0+
* LLVM >= 8.0
* Python 3.x.x (to run the tester)

Follow the steps below to compile this project:
0. Navigate to the root directory of this repository after cloning it.
1. In the `CMakeLists.txt`, specify the path to `llvm x` and set the path to the LLVM_INS variable.
2. Set the version of LLVM that you use for finding the CMake package of the appropriate version.
3. `mkdir build && cd build && cmake ..`
4. `make`

Assuming the build was successful, you will see a directory called `lib` containing the shared object libraries for each of the built passes.

## Run
The passes may either be run manually by using the LLVM's `opt` tool:
`opt -load-pass-plugin <path-to-shared-object-file> -passes=<name-of-pass> <input-IR-file> -S -o <output-IR-file> -debug`

Alternatively, the tester may also be used to optimize a collection of files:
1. Configure `tests/config.json`:
    1. `tests-dir`: full path to the directory containing the test suite for the pass. The directory must contain the directories `input` and `output`, where the `input` directory contains `*.ll` files which will be optimized.
    2. `opt`: full path to the LLVM's optimizer executable.
    3. `lib-path`: path to the pass' shared object file
2. Run `python3 tests/tester.py tests/config.json`
