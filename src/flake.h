#ifndef emilir_flake_h
#define emilir_flake_h

#include <vector>

namespace emilir {

  class flake {
  private:
    const std::vector<uint8_t> _machineNumber;
    const size_t _machineNumberLength;
    const size_t _sequenceNumberLength;
    const size_t _timestampLength;
    const uint64_t _timeOffset;

    uint64_t _lastTimestamp;
    uint64_t _sequenceNumber;
    bool _sequenceNumberOverflow;

  public:
    flake(const uint8_t *machineNumber,
          size_t machineNumberLength,
          size_t sequenceNumberLength,
          size_t timestampLength,
          uint64_t timeOffset);

    inline size_t bit_length() const {
      return _machineNumberLength+_sequenceNumberLength+_timestampLength;
    }

    inline size_t byte_length() const {
      return (bit_length()+7)/8;
    }

    // Return value of 0 means success.
    // Return value of 1 means sequence number overflow or that the
    // timer has moved backwards.
    // Return value of 2 means that the timer has moved backwards
    // by more than 1 second.
    // Return value of 3 means buffer was too small.
    int generate(size_t bufLen, uint8_t *buf);
  };

}

#endif
