BUILD_DIR="./build"
CXX="clang++"

mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR"

export CXX
cmake ..
cmake --build . --target clean
cmake --build .
mv "./compile_commands.json" "../compile_commands.json"
