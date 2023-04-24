pushd ./bin/lrelease
make -C ../../
rm -rf test_build_dir/
mkdir test_build_dir/
./buildTripleBitFromN3 tailored_data.n3 test_build_dir/
./triplebitQuery test_build_dir/ test_queries/
popd
