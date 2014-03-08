# emiflake

[![Dependency status](https://david-dm.org/pereckerdal/emiflake.png)](https://david-dm.org/pereckerdal/emiflake)
[![devDependency Status](https://david-dm.org/pereckerdal/emiflake/dev-status.png)](https://david-dm.org/pereckerdal/emiflake#info=devDependencies)

[![NPM](https://nodei.co/npm/emiflake.png?downloads=true)](https://npmjs.org/package/emiflake)

`emiflake` is a node.js utility library for creating a service that generates unique IDs at scale. Think of it as containing the common logic of [Twitter Snowflake](https://github.com/twitter/snowflake) and [Boundary Flake](https://github.com/boundary/flake), sans network and machine ID generation stuff.

`emiflake` is useful if you want something like Snowflake, but don't want to use Zookeeper to coordinate the machine ID allocation. Another situation where the library is useful is when you want something like [Boundary Flake](https://github.com/boundary/flake), but don't want to add a dependency to Erlang.

It is written in C++ as a node.js extension, because I am very uncomfortable trying to handle 64 bit integers with Javascript's floating point math. `flake.h` and `flake.cc` contain the actual logic, and are usable by themselves, without node.js.

The current implementation uses the C standard function `gettimeofday` to get the time.

## Anatomy

The length of `emiflake` ids vary depending on the parameters: They are `timestampLength`+`sequenceNumberLength`+`machineNumberLength` bits long.

The structure of the ids are: The most significant bits contain the timestamp. After that is the sequence number, and last is the machine number.

## When is it appropriate to use `emiflake` ids?

`emiflake` ids are appropriate when using node.js and when [Boundary Flake](https://github.com/boundary/flake) ids or [Twitter Snowflake](https://github.com/twitter/snowflake) ids would be appropriate. The exact details are well documented on their respective READMEs.

## Recommended parameters

To replicate the beavior of [Boundary Flake](https://github.com/boundary/flake), use parameters like this:

* `machineNumberLength`: 48, and use a MAC address as the machine number.
* `sequenceNumberLength`: 16
* `timestampLength`: 64
* `timestampOffset`: 0

To do something similar to [Twitter Snowflake](https://github.com/twitter/snowflake), use parameters like this:

* `machineNumberLength`: 10, and use some coordination mechanism to choose the machine number.
* `sequenceNumberLength`: 12
* `timestampLength`: 41
* `timestampOffset`: Something like (2012-1970)*365*24*60*60*1000

## Getting started

Install with `npm`:

    npm install emiflake

Generate an id:

    var emiFlake = require('emiflake');
    
    var f = new emiFlake(new Buffer("mid"),
                         /*machineNumberLength:*/24,
                         /*sequenceNumberLength:*/16,
                         /*timestampLength:*/24,
                         /*timestampOffset:*/20000);
    
    f.generate(console.log);

## API

### new emiFlake(machineNumber, machineNumberLength, sequenceNumberLength, timestampLength[, timestampOffset])

Create a new flake object.

* `machineNumber` must be a Buffer object
* `machineNumberLength` should be an integer specifying how many bits of the `machineNumber` that should be used.
* `sequenceNumberLength` should be an integer specifying how many bits the sequence number should use. Must be less than or equal to 64.
* `timestampLength` should be an integer specifying how many bits the timestamp should use. Must be less than or equal to 64.
* `timestampOffset` specifies the epoch, as a positive number of milliseconds since 00:00 Jan 1, 1970. Optional. The default value is 0. When using a small timestamp length, this parameter is useful to maximize the range. For instance, one could set the 0 timestamp to be at the start of 2012.

### EmiFlake.generate(cb)

Generates a unique ID. `cb` should be a callback that takes the resulting ID as a buffer. In case of sequence number overflow or minor backward jumps in the wall time, it will wait and try again. If the wall time has jumped backwards a lot, `cb` will be invoked with `null`.

`generate` never invokes `cb` before it returns.

Unspecified return value.

### EmiFlake.byteLength()

Returns the size of ID buffers in bytes. This is equal to ceil([the bit length]/8).

### EmiFlake.bitLength()

Returns the size of the IDs in bits.
