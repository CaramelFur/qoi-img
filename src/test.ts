import { Readable, Transform } from 'stream';
import fs, { createReadStream, fstat } from 'fs';

const addon = require('bindings')('qoixx_addon');

// {
//   const result = addon.create_encode_state();

//   console.log(result);
// }

const filereading = createReadStream(
  '/home/rubikscraft/Documents/VSCode/Picsur-Project/qoi-stream/img.qoi',
);

const decoder = () => {
  const state = addon.createDecodeState();
  return new Transform({
    transform(chunk, encoding, callback) {
      try {
        const result = addon.streamDecode(state, chunk);
        callback(null, result);
      } catch (e) {
        callback(e as Error);
      }
    },
  });
};

const encoder = (
  width: number,
  height: number,
  channels: 3 | 4,
  colorspace: 0 | 1 = 0,
) => {
  const state = addon.createEncodeState(width, height, channels, colorspace);
  return new Transform({
    transform(chunk, encoding, callback) {
      try {
        const result = addon.streamEncode(state, chunk);
        callback(null, result);
      } catch (e) {
        callback(e as Error);
      }
    },
  });
};

const outputfile = fs.createWriteStream('./hello.qoi');

filereading
  .pipe(decoder())
  .pipe(encoder(12096, 9072, 3, 1))
  .on('end', () => {
    console.log('end');
  })
  .pipe(outputfile);
