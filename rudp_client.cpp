// rudp_client.cpp
// Usage: ./rudp_client <server_ip> <server_port> <payload_length> <num_frames>
//
// Sends num_frames packets of payload_length bytes to server (RUDP).
// Waits for ACK per-packet. Retransmits on timeout. Records per-packet RTT stats.

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <chrono>
#include <ctime>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "data_analysis.h"

struct RudpAck {
    uint32_t ack_seq;       // network byte order
    uint32_t bytes_received; // network byte order
};

static inline uint64_t diff_ns(const struct timespec &a, const struct timespec &b) {
    return (b.tv_sec - a.tv_sec) * 1000000000ULL + (b.tv_nsec - a.tv_nsec);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port> <payload_length> <num_frames>\n";
        return 1;
    }

    std::string server_ip = argv[1];
    int server_port = std::stoi(argv[2]);
    int payload_len = std::stoi(argv[3]);
    uint64_t num_frames = std::stoull(argv[4]);

    if (payload_len <= 0 || payload_len > 60000) {
        std::cerr << "payload_length must be >0 and reasonably small (e.g. <=60000)\n";
        return 1;
    }

    // socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    // recv timeout (in microseconds). Tune this for your network.
    const int TIMEOUT_MS = 200; // 200ms default RTO
    struct timeval tv;
    tv.tv_sec = TIMEOUT_MS / 1000;
    tv.tv_usec = (TIMEOUT_MS % 1000) * 1000;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
        perror("setsockopt SO_RCVTIMEO");
    }

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip.c_str(), &srv.sin_addr) != 1) {
        std::cerr << "inet_pton failed for " << server_ip << "\n";
        return 1;
    }
    socklen_t srvlen = sizeof(srv);

    // Prepare packet buffer: 4 bytes seq (network order) + payload_len bytes data
    const size_t packet_size = sizeof(uint32_t) + payload_len;
    std::vector<char> packet_buf(packet_size);
    std::vector<char> payload(payload_len);
    std::memset(payload.data(), 'A', payload_len);

    DataAnalysis analysis;

    uint64_t successful = 0;
    for (uint32_t seq = 1; seq <= (uint32_t)num_frames; ++seq) {
        // Fill packet
        uint32_t net_seq = htonl(seq);
        memcpy(packet_buf.data(), &net_seq, sizeof(uint32_t));
        memcpy(packet_buf.data() + sizeof(uint32_t), payload.data(), payload_len);

        bool acked = false;
        int retransmits = 0;

        while (!acked) {
            struct timespec t_start, t_end;
            clock_gettime(CLOCK_MONOTONIC, &t_start);
            ssize_t sent = sendto(sock, packet_buf.data(), packet_size, 0, (sockaddr*)&srv, srvlen);
            if (sent < 0) {
                perror("sendto");
                // optionally break or retry
            }

            RudpAck ack;
            ssize_t n = recvfrom(sock, &ack, sizeof(ack), 0, NULL, NULL);
            if (n >= (ssize_t)sizeof(RudpAck)) {
                clock_gettime(CLOCK_MONOTONIC, &t_end);
                uint32_t ack_seq = ntohl(ack.ack_seq);
                uint32_t bytes_rcvd = ntohl(ack.bytes_received);
                if (ack_seq == seq) {
                    uint64_t rtt_ns = diff_ns(t_start, t_end);
                    analysis.mapRange(rtt_ns);
                    analysis.isMin(rtt_ns);
                    analysis.isMax(rtt_ns);
                    analysis.accumulate(rtt_ns);
                    successful++;
                    acked = true;
                } else {
                    // ignore wrong ack (could be old delayed ack)
                }
            } else {
                // timeout or error -> retransmit
                if (errno == EWOULDBLOCK || errno == EAGAIN || n < 0) {
                    // timeout or recv error
                    retransmits++;
                    if (retransmits % 10 == 0) {
                        std::cerr << "seq " << seq << " retransmit count: " << retransmits << "\n";
                    }
                    // loop to retransmit
                } else {
                    // other error
                    perror("recvfrom");
                }
            }
        } // end while for this seq
    } // end for frames

    analysis.findAverage(successful);
    double avg_ms = (double)analysis.getAverage() / 1e6;
    std::cout << "\nRUDP Test Completed\n";
    std::cout << "Frames requested: " << num_frames << "\n";
    std::cout << "Frames successful: " << successful << "\n";
    std::cout << "Payload size (bytes): " << payload_len << "\n";
    std::cout << "Min RTT (ms): " << (analysis.getMin() / 1e6) << "\n";
    std::cout << "Max RTT (ms): " << (analysis.getMax() / 1e6) << "\n";
    std::cout << "Avg RTT (ms): " << avg_ms << "\n";

    // log to file
    char fname[128];
    snprintf(fname, sizeof(fname), "rudp_%u_%llu.log", payload_len, (unsigned long long)num_frames);
    analysis.logToFile(payload_len, num_frames, std::string(fname));
    std::cout << "Log saved to " << fname << "\n";

    close(sock);
    return 0;
}
