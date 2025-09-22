# ReliableUDP
This project implements a reliable UDP protocol for measuring network performance, latency, and jitter. It includes a server and client application written in C++.

Features
Reliable UDP packet transmission with per-packet ACK and retransmission
RTT (Round Trip Time) statistics: min, max, average
Configurable payload size and frame count
Logging of results for analysis


o Compile both server and client using GCC:

    Compile the server code using the following command in the terminal:

    g++ -o udp_serv udp_serv.cpp data_analysis.cpp
    g++ -o rudp_client rudp_client.cpp data_analysis.cpp

o Running:

    Start the server:
     ./udp_serv <port>
   
    Example:
     ./udp_serv 1000

    Run the client:
     ./rudp_client <server_ip> <server_port> <payload_length> <num_frames>

    Example:
     ./rudp_client 127.0.0.1 1000 64 1000000
