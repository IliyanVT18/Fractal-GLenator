if [ ! -d "build" ]; then
    mkdir build
fi
cmake -S . -B ./build
cd ./build
make
bspc rule -a '*' -o state=floating      # REMOVE THIS EVENTUALLY
./fractal-glenator
