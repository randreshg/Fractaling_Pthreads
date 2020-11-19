fractaling: fractaling.c lib/tpool.c lib/xmem.c
	gcc -pthread lib/tpool.c lib/xmem.c fractaling.c -o fractaling

#fractaling.o: fractaling.c
#	gcc -c -pthread fractaling.c

#tpool.o: tpool.c
#	gcc -c -pthread tpool.c

#xmem.o: xmem.c
#	gcc -c xmem.c

#clean:
#	rm *.o fractaling fractaling.ppm