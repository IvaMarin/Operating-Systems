all: client computing_node

client:
	g++ zmq_client.cpp -lzmq -o client -Wall -pedantic

computing_node:
	g++ computing_node.cpp -lzmq -o computing_node -Wall -pedantic

clean:
	rm -rf client computing_node