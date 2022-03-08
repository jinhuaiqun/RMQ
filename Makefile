all: broker producer consumer

producer:producer.o 
	gcc -o producer producer.o
	mkdir -p bin/.
	mv producer bin/.
	rm producer.o

producer.o: producer.c 
	gcc -c producer.c

consumer:consumer.o 
	gcc -o consumer consumer.o
	mkdir -p bin/.
	mv consumer bin/.
	rm consumer.o

consumer.o: consumer.c 
	gcc -c consumer.c

broker:broker.o
	gcc -o broker broker.o
	mkdir -p bin/
	mv broker bin/.
	rm broker.o

broker.o:broker.c
	gcc -c broker.c


clean:
	rm -rf bin/ msg/
