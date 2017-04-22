	
one: numberOne.c
	mpicc numberOne.c -o one
2d: transpose2d_driver.c
	mpicc transpose2d_driver.c -o 2d
1d: transpose1d_driver.c
	mpicc transpose1d_driver.c -o 1d

all: 2d 1d one

runOne:
	ibrun -np 8 ./one
run1d: 1d
	ibrun -np 2 ./1d 8
run2d: 2d
	ibrun -np 16 ./2d 16
runAll: 2d 1d one
	make runOne
	make run1d
	make run2d