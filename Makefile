

all:
	g++ common.cpp timer.cpp -g -lpthread -o common
clean:
	rm -f common
