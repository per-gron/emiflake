var EmiFlakeAddon = require('../build/Release/emiflake');

var EmiFlake = function(machineNumber,
                        machineNumberLength,
                        sequenceNumberLength,
                        timestampLength,
                        timeOffset) {
  if (!timeOffset) timeOffset = 0;

  this._handle = new EmiFlakeAddon.EmiFlake(machineNumber,
                                            machineNumberLength,
                                            sequenceNumberLength,
                                            timestampLength,
                                            timeOffset);
  
  this.machineNumber = machineNumber;
  this.machineNumberLength = machineNumberLength;
  this.sequenceNumberLength = sequenceNumberLength;
  this.timestampLength = timestampLength;
  this.timeOffset = timeOffset;
};

EmiFlake.prototype.generate = function(cb) {
  var buf = new Buffer(this._handle.byteLength());
  buf.fill(0);
  
  var result = this._handle.generate(buf);
  var obj = this;

  if (0 == result) {
    // Success
    process.nextTick(function() {
      cb(buf);
    });
  }
  else if (1 == result) {
    // Sequence number overflow. Wait for 1 ms and try again
    setTimeout(function() {
      obj.generate(cb);
    }, 1);
  }
  else if (2 == result) {
    // The timer has moved backwards by more than 1 second
    process.nextTick(function() {
      cb(null);
    });
  }
  else {
    // Should not happen
    throw new Error("Internal error in EmiFlake");
  }
};

EmiFlake.prototype.byteLength = function() {
  return this._handle.byteLength();
};

EmiFlake.prototype.bitLength = function() {
  return this._handle.bitLength();
};

module.exports = EmiFlake;
