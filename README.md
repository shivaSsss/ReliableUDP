# ReliableUDP
This project implements a reliable UDP protocol for measuring network performance, latency, and jitter. It includes a server and client application written in C++.

Features
Reliable UDP packet transmission with per-packet ACK and retransmission
RTT (Round Trip Time) statistics: min, max, average
Configurable payload size and frame count
Logging of results for analysis
**#Building**
o Compile both server and client using GCC:

    Compile the server code using the following command in the terminal:

    g++ -o udp_serv udp_serv.cpp data_analysis.cpp
    g++ -o rudp_client rudp_client.cpp data_analysis.cpp

Running
Start the server:
./udp_serv <port>

Example:
./udp_serv 1000

Run the client:
./rudp_client <server_ip> <server_port> <payload_length> <num_frames>
Example:
./rudp_client 127.0.0.1 1000 64 1000000
Output
The client prints RTT statistics and saves logs to a file (e.g., rudp_64_1000000.log).


**# How to Run**

o run the UDP_SERVER:

    Compile the server code using the following command in the terminal:

    gcc udp_server.c -lpthread -o udp_serv

Execute the compiled server program:

./udp_serv 1000

To run the UDP_CLIENT:

    Compile the client code using the following command in the terminal:

    gcc udp_client.c -lpthread -o udp_cli

Execute the compiled client program, providing the server IP address as a command-line argument (in this example, it is set to 127.0.0.1):

./udp_cli 127.0.0.1

Note: Ensure that the required dependencies, including the gcc compiler, are installed on your Linux system before running the commands.
