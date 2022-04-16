import { readFileSync } from 'fs';
import { QOIChannels, QOIColorSpace, QOIdecode } from '../';
import Image from './resources/image.json';

const rgb_qoi = readFileSync('./test/resources/rgb_image.qoi');
const rgba_qoi = readFileSync('./test/resources/rgba_image.qoi');
const rgb_raw = readFileSync('./test/resources/rgb_image.rgb');
const rgba_raw = readFileSync('./test/resources/rgba_image.rgb');

test('Decode RGB qoi file', () => {
  const result = QOIdecode(rgb_qoi);

  expect(typeof result).toBe('object');

  expect(result.pixels.equals(rgb_raw)).toBe(true);
  expect(result.width).toBe(Image.width);
  expect(result.height).toBe(Image.height);

  expect(result.channels).toBe(3);
  expect(result.colorspace).toBe(QOIColorSpace.SRGB);
});

test('Decode RGBA qoi file', () => {
  const result = QOIdecode(rgba_qoi);

  expect(typeof result).toBe('object');

  expect(result.pixels.equals(rgba_raw)).toBe(true);
  expect(result.width).toBe(Image.width);
  expect(result.height).toBe(Image.height);

  expect(result.channels).toBe(4);
  expect(result.colorspace).toBe(QOIColorSpace.SRGB);
});

test('Decode RGB qoi file as RGBA', () => {
  const result = QOIdecode(rgb_qoi, QOIChannels.RGBA);

  expect(typeof result).toBe('object');

  expect(result.pixels.equals(rgba_raw)).toBe(true);
  expect(result.width).toBe(Image.width);
  expect(result.height).toBe(Image.height);

  expect(result.channels).toBe(4);
  expect(result.colorspace).toBe(QOIColorSpace.SRGB);
});

test('Decode RGBA qoi file as RGB', () => {
  const result = QOIdecode(rgba_qoi, QOIChannels.RGB);

  expect(typeof result).toBe('object');

  expect(result.pixels.equals(rgb_raw)).toBe(true);
  expect(result.width).toBe(Image.width);
  expect(result.height).toBe(Image.height);

  expect(result.channels).toBe(3);
  expect(result.colorspace).toBe(QOIColorSpace.SRGB);
});
