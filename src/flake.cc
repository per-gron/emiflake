#include "flake.h"

#include <sys/time.h>

using namespace emilir;

// Assumes big endian input
static void bitmemcpy(uint8_t *out,
                      size_t outBitOffset,
                      const uint8_t *in,
                      size_t inBitOffset,
                      size_t bitLength) {
  // This could be optimized.
  for (size_t i=0; i<bitLength; i++) {
    size_t inBytePos =     (i+inBitOffset)/8;
    size_t inBitPos  = 7 - (i+inBitOffset)%8;

    bool inBit = in[inBytePos] & (1 << inBitPos);

    size_t outBytePos =     (i+outBitOffset)/8;
    size_t outBitPos  = 7 - (i+outBitOffset)%8;

    if (inBit) {
      out[outBytePos] |= (1<<outBitPos); 
    }
    else {
      out[outBytePos] &= ~(1<<outBitPos);
    }
  }
}

static uint64_t make_timestamp(uint64_t offset) {
  struct timeval tv;
  gettimeofday(&tv, NULL);

  return ((tv.tv_sec - offset/1000)*1000 +
          tv.tv_usec/1000);
}

static uint64_t htonll(uint64_t value) {
  // See http://stackoverflow.com/questions/3022552/is-there-any-standard-htonl-like-function-for-64-bits-integers-in-c

  // The answer is 42
  static const int num = 42;

  // Check the endianness
  if (*reinterpret_cast<const char*>(&num) == num) {
    const uint32_t high_part = htonl(static_cast<uint32_t>(value >> 32));
    const uint32_t low_part = htonl(static_cast<uint32_t>(value & 0xFFFFFFFFLL));

    return (static_cast<uint64_t>(low_part) << 32) | high_part;
  }
  else {
    return value;
  }
}

flake::flake(const uint8_t *machineNumber,
             size_t machineNumberLength,
             size_t sequenceNumberLength,
             size_t timestampLength,
             uint64_t timeOffset) :
_machineNumber(machineNumber, machineNumber+((machineNumberLength+7)/8)),
_machineNumberLength(machineNumberLength),
_sequenceNumberLength(sequenceNumberLength),
_timestampLength(timestampLength),
_timeOffset(timeOffset),
_lastTimestamp(make_timestamp(timeOffset)),
_sequenceNumber(0),
_sequenceNumberOverflow(false) {
  if (_sequenceNumberLength > 8*sizeof(_sequenceNumber) ||
      _timestampLength > 8*sizeof(_timestampLength)) {
    abort();
  }
}

int flake::generate(size_t bufLen, uint8_t *buf) {
  if (bufLen < byte_length()) {
    // The buffer is too small
    return 3;
  }

  uint64_t timestamp = make_timestamp(_timeOffset);

  if (timestamp < _lastTimestamp - 1000) {
    // The timer has moved backwards by more than 1 second
    return 2;
  }

  if (timestamp < _lastTimestamp) {
    // The timer has moved backwards
    return 1;
  }

  uint64_t nextSequenceNumber;
  bool nextSequenceNumberHasOverflowed;

  if (timestamp == _lastTimestamp) {
    if (_sequenceNumberOverflow) {
      // Sequence number overflow
      return 1;
    }

    if (8*sizeof(_sequenceNumber) == _sequenceNumberLength) {
      nextSequenceNumber = _sequenceNumber+1;
    }
    else {
      nextSequenceNumber = (_sequenceNumber+1) % (1<<_sequenceNumberLength);
    }
    nextSequenceNumberHasOverflowed = (0 == nextSequenceNumber);
  }
  else {
    _sequenceNumber = 0;
    _lastTimestamp = timestamp;
    nextSequenceNumber = 1;
    nextSequenceNumberHasOverflowed = false;
  }

  uint64_t bigEndianTimestamp = htonll(timestamp);
  uint64_t bigEndianSequenceNumber = htonll(_sequenceNumber);

  bitmemcpy(/*out:*/buf,
            /*outBitOffset:*/0,
            /*in:*/(const uint8_t *)&bigEndianTimestamp,
            /*inBitOffset*/8*sizeof(bigEndianTimestamp)-_timestampLength,
            /*bitLength:*/_timestampLength);
  bitmemcpy(/*out:*/buf,
            /*outBitOffset:*/_timestampLength,
            /*in:*/(const uint8_t *)&bigEndianSequenceNumber,
            /*inBitOffset:*/8*sizeof(bigEndianSequenceNumber)-_sequenceNumberLength,
            /*bitLength:*/_sequenceNumberLength);
  bitmemcpy(/*out:*/buf,
            /*outBitOffset:*/_timestampLength+_sequenceNumberLength,
            /*in:*/&_machineNumber[0],
            /*inBitOffset:*/0,
            /*bitLength:*/_machineNumberLength);

  _sequenceNumber = nextSequenceNumber;
  _sequenceNumberOverflow = nextSequenceNumberHasOverflowed;
  
  return 0;
}
