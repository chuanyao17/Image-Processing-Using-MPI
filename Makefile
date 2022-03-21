img_mpi: main.o img_processing.o matrix_para.o 
	gcc -Wall main.o img_processing.o matrix_para.o -o img_mpi

main.o: main.cpp
	gcc -c main.cpp

matrix_para.o: matrix_para.cpp matrix_para.h
	gcc -c matrix_para.cpp

img_processing.o: img_processing.cpp img_processing.h
	gcc -c img_processing.cpp





clean:
	rm *.o img_mpi
