img_mpi: main.o img_processing.o matrix_para.o 
	gcc -Wall main.o img_processing.o matrix_para.o -o img_mpi

main.o: main.c
	gcc -c main.c

matrix_para.o: matrix_para.c matrix_para.h
	gcc -c matrix_para.c

img_processing.o: img_processing.c img_processing.h
	gcc -c img_processing.c





clean:
	rm *.o img_mpi
