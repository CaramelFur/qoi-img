import { Readable } from 'stream';

const addon = require('bindings')('qoixx_addon');

{
  const result = addon.create_encode_state();

  console.log(result);
}

console.log("Hello, world!");


const b = Buffer.from("Hello, world!");

const s = new Readable({
  read() {
    this.push(b);
    this.push(null);
  }
})
