cmake . -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
readelf -S pl0 | grep debug