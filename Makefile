all:
	gcc -pthread -o server main.c network_info.c udp_multicast.c tcp_connection.c
clean:
	rm server