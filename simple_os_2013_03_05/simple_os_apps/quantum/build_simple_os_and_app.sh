pushd .
cd ../../simple_os
./make_lib_clean
./make_lib_x86_host
popd
make -f Makefile_32 clean
make -f Makefile_32
