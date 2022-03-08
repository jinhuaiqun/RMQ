all: broker client

client:client.o
	gcc -o client client.o
	mkdir -p bin/.
	mv client bin/.
	rm client.o

client.o: client.c
	gcc -c client.c

broker:broker.o
	gcc -o broker broker.o
	mkdir -p bin/
	mv broker bin/.
	rm broker.o

broker.o:broker.c
	gcc -c broker.c


clean:
	rm -rf bin/
