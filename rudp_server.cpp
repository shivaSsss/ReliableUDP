// rudp_server.cpp
// Simple Reliable-UDP (ACK) server that responds with ACK containing seq and bytes received.

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

struct RudpAck {
    uint32_t ack_seq;       // network byte order
    uint32_t bytes_received; // network byte order
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }
    int port = std::stoi(argv[1]);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(port);

    if (bind(sock, (sockaddr*)&srv, sizeof(srv)) < 0) { perror("bind"); return 1; }

    std::cout << "RUDP server listening on port " << port << "...\n";

    const size_t MAX_PKT = 65536;
    char *recvbuf = (char*)malloc(MAX_PKT);
    if (!recvbuf) { perror("malloc"); return 1; }

    sockaddr_in cli{};
    socklen_t cli_len = sizeof(cli);

    while (true) {
        ssize_t n = recvfrom(sock, recvbuf, MAX_PKT, 0, (sockaddr*)&cli, &cli_len);
        if (n < 0) { perror("recvfrom"); continue; }
        if (n < (ssize_t)sizeof(uint32_t)) {
            // invalid packet
            continue;
        }
        // first 4 bytes are seq in network order
        uint32_t net_seq;
        memcpy(&net_seq, recvbuf, sizeof(uint32_t));
        uint32_t seq = ntohl(net_seq);
        uint32_t bytes_received = (uint32_t)(n - sizeof(uint32_t));

        // Build ACK
        RudpAck ack;
        ack.ack_seq = htonl(seq);
        ack.bytes_received = htonl(bytes_received);

        // Send ACK back
        ssize_t sent = sendto(sock, &ack, sizeof(ack), 0, (sockaddr*)&cli, cli_len);
        if (sent < 0) {
            perror("sendto");
            continue;
        }

        // (Optional) If you want server to echo payload, uncomment:
        // sendto(sock, recvbuf + sizeof(uint32_t), bytes_received, 0, (sockaddr*)&cli, cli_len);
    }

    free(recvbuf);
    close(sock);
    return 0;
}
