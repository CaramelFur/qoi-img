export enum QoiColorSpace {
  SRGB = 0,
  Linear = 1,
}

type addonType = {
  encode: (
    pixels: Buffer,
    width: number,
    height: number,
    channels: number,
    colorSpace: QoiColorSpace,
  ) => Buffer;
  decode: (data: Buffer) => {
    pixels: Buffer;
    width: number;
    height: number;
    channels: number;
    colorSpace: QoiColorSpace;
  };
};

const addon = require('bindings')('qoixx_addon') as addonType;

const QOIbytes = Buffer.from('qoif');

export function QOIencode(
  pixels: Buffer,
  options: {
    width: number;
    height: number;
    channels: number;
    colorSpace?: QoiColorSpace;
  },
): Buffer {
  return addon.encode(
    pixels,
    options.width,
    options.height,
    options.channels,
    options.colorSpace || QoiColorSpace.SRGB,
  );
}

export const QOIdecode = addon.decode;

export function IsQOI(data: Buffer): boolean {
  return (
    data.length >= QOIbytes.length &&
    data.slice(0, QOIbytes.length).equals(QOIbytes)
  );
}
