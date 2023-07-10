rm -r build
mkdir -p build
pushd ./build > /dev/null
cmake .. -DCMAKE_INSTALL_PREFIX=../
make && make install
popd > /dev/null
pushd bin > /dev/null
chmod +x tin
