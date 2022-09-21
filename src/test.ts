const addon = require('bindings')('qoixx_addon');

const result = addon.encodestream();

console.log(result);
console.log(result());
console.log(result());
console.log(result());

const result2 = addon.encodestream();

console.log(result2);
console.log(result2());
console.log(result2());
console.log(result2());