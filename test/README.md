cd ..

cd tagha

make

cd ..

mv tagha/libtagha.a .

make

./taghatest test_asm/test_fib40.tbc

7.5s

cd test_asm

../taghatest test_dynamicloading.tbc

../taghatest test_dynamiclinking.tbc
