rm build -rf
mkdir build
cd build
export CC=/usr/bin/mpicc
export CXX=/usr/bin/mpic++
cmake -DCMAKE_CXX_COMPILER=/usr/bin/mpic++ ../
make
cp ../image.jpg . 
cp ../100x300_image.jpg . 
cp ../100x210_image.jpg .
cp ../test.jpg .
cp ../3x2_all_b.jpg .
cp ../3x6_b_g_w.jpg .
cp ../1x6.jpg .
cp ../women.png .

