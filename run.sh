pushd ./bin/lrelease
make -w -C ../../
rm -rf test_build_dir/
mkdir test_build_dir/
./buildTripleBitFromN3 1m.n3 test_build_dir/
./triplebitQuery test_build_dir/ test_queries2/
popd
