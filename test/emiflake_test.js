var chai = require('chai');
var EmiFlake = require('..');
var expect = chai.expect;

describe('EmiFlake', function() {
  it('generates IDs', function(done) {
    var f = new EmiFlake(new Buffer("mid"), 24, 16, 24, 20000);
    f.generate(function(id) {
      expect(id.length).to.eql(8);
      done()
    });
  })
})