rm build -rf
mkdir build
cd build
export CC=/usr/bin/mpicc
export CXX=/usr/bin/mpic++
cmake -DCMAKE_CXX_COMPILER=/usr/bin/mpic++ ../
make

##cp testing pics
cp ../300x100_b_g_w_test.jpg . 
cp ../210x100_b_g_w_test.jpg .
cp ../3x2_all_black_test.jpg .
cp ../3x6_b_g_w_test.jpg .
cp ../1x6_test.jpg .
cp ../100x210_left_test.jpg .
cp ../keyboard.jpg . 
cp ../women.png .
cp ../logo.jpg .
