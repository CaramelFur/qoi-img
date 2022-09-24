import { streamToBuffer } from '@jorgeferrero/stream-to-buffer';
import { readFileSync } from 'fs';
import { QOIChannels, QOIColorSpace, QOIEncoder } from '../dist';
import Image from './resources/image.json';
import { Streamify } from './streamify';

const rgb_qoi = readFileSync('./test/resources/rgb_image.qoi');
const rgba_qoi = readFileSync('./test/resources/rgba_image.qoi');
const rgb_raw = readFileSync('./test/resources/rgb_image.rgb');
const rgba_raw = readFileSync('./test/resources/rgba_image.rgb');

test('Stream encode RGB qoi file', async () => {
  const encoder = new QOIEncoder(
    Image.width,
    Image.height,
    QOIChannels.RGB,
    QOIColorSpace.SRGB,
  );

  const inputStream = Streamify(rgb_raw);
  const outputStream = inputStream.pipe(encoder);

  const output = await streamToBuffer(outputStream);
  expect(output.equals(rgb_qoi)).toBe(true);
});

test('Stream encode RGB qoi file', async () => {
  const encoder = new QOIEncoder(
    Image.width,
    Image.height,
    QOIChannels.RGBA,
    QOIColorSpace.SRGB,
  );

  const inputStream = Streamify(rgba_raw);
  const outputStream = inputStream.pipe(encoder);

  const output = await streamToBuffer(outputStream);
  expect(output.equals(rgba_qoi)).toBe(true);
});
