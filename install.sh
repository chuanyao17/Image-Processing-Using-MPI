rm build -rf
mkdir build
cd build
export CC=/usr/bin/mpicc
export CXX=/usr/bin/mpic++
cmake -DCMAKE_CXX_COMPILER=/usr/bin/mpic++ ../
make

##cp testing images to the /build
cp ../testing_images/300x100_b_g_w_test.jpg . 
cp ../testing_images/210x100_b_g_w_test.jpg .
cp ../testing_images/3x2_all_black_test.jpg .
cp ../testing_images/3x6_b_g_w_test.jpg .
cp ../testing_images/1x6_test.jpg .
cp ../testing_images/100x210_left_test.jpg .
cp ../testing_images/keyboard.jpg . 
cp ../testing_images/women.png .
cp ../testing_images/logo.jpg .
