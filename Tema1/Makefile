build: tema_atm.o 
	gcc -Wall tema_atm.o -o atm
tema_atm.o: tema_atm.c
	gcc -c -Wall tema_atm.c
run: build
	./atm
clean:
	rm -f *.o
	rm -f atm