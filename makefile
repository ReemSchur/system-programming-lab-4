all: hexeditplus task4

hexeditplus: task1.c
	gcc -m32 -g -Wall task1.c -o hexeditplus

task4: task4.c
	gcc -m32 -fno-pie -fno-stack-protector -g -Wall task4.c -o task4

clean:
	rm -f hexeditplus task4