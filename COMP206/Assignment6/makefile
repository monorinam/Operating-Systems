program : main.o producer.o consumer.o
		gcc -o program main.o producer.o consumer.o
main.o  : main.c producer.o consumer.o
		gcc -Wall -g -c main.c
producer.o  : producer.c main.h
		gcc -Wall -g -c producer.c
consumer.o  : consumer.c main.h
		gcc -Wall -g -c consumer.c
clean	: 
		rm -f program main.o producer.o consumer.o TURN.txt DATA.txt
