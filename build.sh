BUILD_DIR="./build"
CXX="clang++"

mkdir -p "$BUILD_DIR"

if [ $# -eq 0 ]; then
    cd "$BUILD_DIR"

    echo "Attempting to build a game"
    export CXX
    cmake ..
    cmake --build . --target clean
    cmake --build .
    echo "Updating compile_commands.json"
    mv "./compile_commands.json" "../compile_commands.json"
else
    if [ "$1" == "format" ]; then
        echo "Attempting to format game's sources"
        find src/ \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -style=file -i {} \;
        find dependencies/engine/ \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -style=file -i {} \;
    else
        echo "Invalid build command, check $0 and try again"
    fi
fi

echo "Done"
