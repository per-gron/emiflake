var EmiFlake = require('./emiflake');

var f = new EmiFlake(new Buffer("hej"),
                     /*machineNumberLength:*/24,
                     /*sequenceNumberLength:*/16,
                     /*timestampLength:*/24,
                     /*timestampOffset:*/20000);

console.log(f, f.byteLength(), f.bitLength());
f.generate(console.log);
f.generate(console.log);
f.generate(console.log);
f.generate(console.log);
f.generate(console.log);
f.generate(console.log);
f.generate(console.log);
f.generate(console.log);
f.generate(console.log);
f.generate(console.log);
