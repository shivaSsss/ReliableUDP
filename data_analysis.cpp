#include "data_analysis.h"
#include <fstream>
#include <iomanip>

DataAnalysis::DataAnalysis() {
    minVal = std::numeric_limits<uint64_t>::max();
    maxVal = 0;
    total = 0.0L;
    samples_count = 0;
    // Define histogram: 100 buckets, each 1000 us (1ms) wide by default (~100ms range).
    hist_bucket_size_ns = 1000000; // 1ms in ns
    histogram_buckets.assign(200, 0); // expandable
    averageVal = 0.0;
}

void DataAnalysis::mapRange(uint64_t sample_ns) {
    uint64_t idx = sample_ns / hist_bucket_size_ns;
    if (idx >= histogram_buckets.size()) {
        // expand buckets if needed
        size_t newSize = idx + 10;
        histogram_buckets.resize(newSize, 0);
    }
    histogram_buckets[idx]++;
}

void DataAnalysis::isMin(uint64_t sample_ns) {
    if (sample_ns < minVal) minVal = sample_ns;
}

void DataAnalysis::isMax(uint64_t sample_ns) {
    if (sample_ns > maxVal) maxVal = sample_ns;
}

void DataAnalysis::accumulate(uint64_t sample_ns) {
    total += (long double) sample_ns;
    samples_count++;
}

void DataAnalysis::findAverage(uint64_t numSamples) {
    if (samples_count == 0) averageVal = 0.0;
    else averageVal = (double)(total / (long double) samples_count);
}

void DataAnalysis::logToFile(size_t payload_len, uint64_t frames, const std::string &filename) {
    std::ofstream ofs(filename, std::ios::out);
    if (!ofs.is_open()) return;
    ofs << "RUDP Test Log\n";
    ofs << "Payload bytes: " << payload_len << "\n";
    ofs << "Frames: " << frames << "\n";
    ofs << "Samples recorded: " << samples_count << "\n";
    ofs << std::fixed << std::setprecision(3);
    ofs << "Min RTT (ms): " << (minVal / 1e6) << "\n";
    ofs << "Max RTT (ms): " << (maxVal / 1e6) << "\n";
    ofs << "Average RTT (ms): " << (averageVal / 1e6) << "\n\n";

    ofs << "Histogram (bucket size = " << (hist_bucket_size_ns / 1e6) << " ms):\n";
    for (size_t i = 0; i < histogram_buckets.size(); ++i) {
        if (histogram_buckets[i] == 0) continue;
        ofs << "  " << (i * (hist_bucket_size_ns / 1000000.0)) << " ms - "
            << ((i + 1) * (hist_bucket_size_ns / 1000000.0)) << " ms : "
            << histogram_buckets[i] << "\n";
    }
    ofs.close();
}

uint64_t DataAnalysis::getMin() const { return minVal; }
uint64_t DataAnalysis::getMax() const { return maxVal; }
double DataAnalysis::getAverage() const { return averageVal; }
