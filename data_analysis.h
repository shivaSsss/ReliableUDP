#ifndef DATA_ANALYSIS_H
#define DATA_ANALYSIS_H

#include <vector>
#include <cstdint>
#include <limits>
#include <string>

class DataAnalysis {
public:
    DataAnalysis();
    void mapRange(uint64_t sample_ns); // put sample into histogram buckets
    void isMin(uint64_t sample_ns);
    void isMax(uint64_t sample_ns);
    void accumulate(uint64_t sample_ns);
    void findAverage(uint64_t numSamples);
    void logToFile(size_t payload_len, uint64_t frames, const std::string &filename);

    uint64_t getMin() const;
    uint64_t getMax() const;
    double getAverage() const;
private:
    uint64_t minVal;
    uint64_t maxVal;
    long double total;
    std::vector<uint64_t> histogram_buckets; // simple histogram
    uint64_t hist_bucket_size_ns; // bucket width
    uint64_t samples_count;
    double averageVal;
};

#endif // DATA_ANALYSIS_H
