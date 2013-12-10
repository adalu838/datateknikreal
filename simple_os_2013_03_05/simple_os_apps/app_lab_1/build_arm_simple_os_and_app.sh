pushd .
cd ../../simple_os_lab_1
./make_lib_arm
popd
make -f Makefile_arm_bb clean
make -f Makefile_arm_bb


