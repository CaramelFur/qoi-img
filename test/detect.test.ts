import { readFileSync } from 'fs';
import { IsQOI } from '../';

const rgb_qoi = readFileSync('./test/resources/rgb_image.qoi');
const rgba_qoi = readFileSync('./test/resources/rgba_image.qoi');
const rgb_png = readFileSync('./test/resources/rgb_image.png');
const rgba_png = readFileSync('./test/resources/rgba_image.png');

test('Detect RGB qoi file', () => {
  const result = IsQOI(rgb_qoi);
  expect(result).toBe(true);
});

test('Detect RGBA qoi file', () => {
  const result = IsQOI(rgba_qoi);
  expect(result).toBe(true);
});

test('Dont detect RGB png file', () => {
  const result = IsQOI(rgb_png);
  expect(result).toBe(false);
});

test('Dont detect RGBA png file', () => {
  const result = IsQOI(rgba_png);
  expect(result).toBe(false);
});
