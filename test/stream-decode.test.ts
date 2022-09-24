import { streamToBuffer } from '@jorgeferrero/stream-to-buffer';
import { readFileSync } from 'fs';
import { QOIChannels, QOIColorSpace, QOIDecoder } from '../dist';
import Image from './resources/image.json';
import { Streamify } from './streamify';

const rgb_qoi = readFileSync('./test/resources/rgb_image.qoi');
const rgba_qoi = readFileSync('./test/resources/rgba_image.qoi');
const rgb_raw = readFileSync('./test/resources/rgb_image.rgb');
const rgba_raw = readFileSync('./test/resources/rgba_image.rgb');

test('Stream decode RGB qoi file', async () => {
  const decoder = new QOIDecoder();

  const inputStream = Streamify(rgb_qoi);
  const outputStream = inputStream.pipe(decoder);

  const output = await streamToBuffer(outputStream);
  expect(output.equals(rgb_raw)).toBe(true);

  const info = decoder.getInfo();

  expect(typeof info).toBe('object');

  expect(info.width).toBe(Image.width);
  expect(info.height).toBe(Image.height);

  expect(info.channels).toBe(3);
  expect(info.colorspace).toBe(QOIColorSpace.SRGB);
});

test('Stream decode RGBA qoi file', async () => {
  const decoder = new QOIDecoder();

  const inputStream = Streamify(rgba_qoi);
  const outputStream = inputStream.pipe(decoder);

  const output = await streamToBuffer(outputStream);
  expect(output.equals(rgba_raw)).toBe(true);

  const info = decoder.getInfo();

  expect(typeof info).toBe('object');

  expect(info.width).toBe(Image.width);
  expect(info.height).toBe(Image.height);

  expect(info.channels).toBe(4);
  expect(info.colorspace).toBe(QOIColorSpace.SRGB);
});

test('Stream decode RGB qoi file as RGBA', async () => {
  const decoder = new QOIDecoder(QOIChannels.RGBA);

  const inputStream = Streamify(rgb_qoi);
  const outputStream = inputStream.pipe(decoder);

  const output = await streamToBuffer(outputStream);
  expect(output.equals(rgba_raw)).toBe(true);

  const info = decoder.getInfo();

  expect(typeof info).toBe('object');

  expect(info.width).toBe(Image.width);
  expect(info.height).toBe(Image.height);

  expect(info.channels).toBe(4);
  expect(info.colorspace).toBe(QOIColorSpace.SRGB);
});

test('Stream decode RGBA qoi file as RGB', async () => {
  const decoder = new QOIDecoder(QOIChannels.RGB);

  const inputStream = Streamify(rgba_qoi);
  const outputStream = inputStream.pipe(decoder);

  const output = await streamToBuffer(outputStream);
  expect(output.equals(rgb_raw)).toBe(true);

  const info = decoder.getInfo();

  expect(typeof info).toBe('object');

  expect(info.width).toBe(Image.width);
  expect(info.height).toBe(Image.height);

  expect(info.channels).toBe(3);
  expect(info.colorspace).toBe(QOIColorSpace.SRGB);
});
