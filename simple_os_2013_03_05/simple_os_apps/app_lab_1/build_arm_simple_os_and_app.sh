pushd .
cd ../../simple_os_lab_1
./make_lib_clean
./make_lib_arm_bb
popd

make -f Makefile_arm_bb