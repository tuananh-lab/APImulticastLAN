# API-Multicast-LAN-
Build a system including 1 server and 2 client machines.
+ The server sends UDP multicast messages one after another to the clients, (The server knows the client's UDP port). The multicast message has the form: "Hello <IP_SERVER><TCP_PORT>"
+ Server listens for TCP connections (TCP_PORT= 7000)
+ The client receives the UDP multicast message from the server, decodes the message, and establishes a TCP connection to the server.
+ Client and Server exchange information with each other. (ex: client sends message "Send from client: <IP_CLIENT>")
To run the server program, perform the following steps:
+Compile the program:
  make
+Run the program:
  ./server
Note that the server and client programs run on different virtual machines.
