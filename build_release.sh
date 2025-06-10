cd Content/Shaders/Source
./compile_shaders.sh
cd ../../..

cmake -B build -DCMAKE_BUILD_TYPE=Release .
cmake --build build
