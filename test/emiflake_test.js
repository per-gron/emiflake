var chai = require('chai');
var emiflake = require('..');
var expect = chai.expect;

describe('emiflake', function() {
  it('generates IDs', function(done) {
    var f = new emiflake(new Buffer("mid"), 24, 16, 24, 20000);
    f.generate(function(id) {
      expect(id.length).to.eql(8);
      done()
    });
  })
})