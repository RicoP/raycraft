g++ ./main.cpp -std=c++17 -o ./raycraft.out -L./raylib/lib/ -lraylib
LD_LIBRARY_PATH=./raylib/lib/
export LD_LIBRARY_PATH
./raycraft.out