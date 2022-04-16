import { readFileSync } from 'fs';
import { QOIChannels, QOIColorSpace, QOIencode } from '../dist';
import Image from './resources/image.json';

const rgb_qoi = readFileSync('./test/resources/rgb_image.qoi');
const rgba_qoi = readFileSync('./test/resources/rgba_image.qoi');
const rgb_raw = readFileSync('./test/resources/rgb_image.rgb');
const rgba_raw = readFileSync('./test/resources/rgba_image.rgb');

test('Encode RGB qoi file', () => {
  const result = QOIencode(rgb_raw, {
    channels: QOIChannels.RGB,
    width: Image.width,
    height: Image.height,
    colorspace: QOIColorSpace.SRGB,
  });

  expect(result instanceof Buffer).toBe(true);

  expect(result.equals(rgb_qoi)).toBe(true);
});

test('Encode RGBA qoi file', () => {
  const result = QOIencode(rgba_raw, {
    channels: QOIChannels.RGBA,
    width: Image.width,
    height: Image.height,
    colorspace: QOIColorSpace.SRGB,
  });

  expect(result instanceof Buffer).toBe(true);

  expect(result.equals(rgba_qoi)).toBe(true);
});
