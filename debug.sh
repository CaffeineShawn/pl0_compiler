cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
readelf -S compiler | grep debug