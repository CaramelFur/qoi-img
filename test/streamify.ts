import { Readable } from 'stream';

export function Streamify(buffer: Buffer, chunksize: number = 64): Readable {
  const stream = new Readable({
    read() {
      if (buffer.length > 0) {
        this.push(buffer.subarray(0, chunksize));
        buffer = buffer.subarray(chunksize);
      } else {
        this.push(null);
      }
    },
  });
  return stream;
}

