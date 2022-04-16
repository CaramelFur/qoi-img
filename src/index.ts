export enum QOIColorSpace {
  SRGB = 0,
  Linear = 1,
}

export enum QOIChannels {
  RGB = 3,
  RGBA = 4,
}

type addonType = {
  encode: (
    pixels: Buffer,
    width: number,
    height: number,
    channels: QOIChannels,
    colorspace: QOIColorSpace,
  ) => Buffer;
  decode: (
    data: Buffer,
    channels: number,
  ) => {
    pixels: Buffer;
    width: number;
    height: number;
    channels: QOIChannels;
    colorspace: QOIColorSpace;
  };
};

const addon = require('bindings')('qoixx_addon') as addonType;

const QOIbytes = Buffer.from('qoif');

export function QOIencode(
  pixels: Buffer,
  options: {
    width: number;
    height: number;
    channels: QOIChannels;
    colorspace?: QOIColorSpace;
  },
): Buffer {
  return addon.encode(
    pixels,
    options.width,
    options.height,
    options.channels,
    options.colorspace || QOIColorSpace.SRGB,
  );
}

export function QOIdecode(
  data: Buffer,
  convertChannels?: QOIChannels,
): {
  pixels: Buffer;
  width: number;
  height: number;
  channels: QOIChannels;
  colorspace: QOIColorSpace;
} {
  const result = addon.decode(data, convertChannels || 0);
  if (convertChannels !== undefined) result.channels = convertChannels;

  return result;
}

export function IsQOI(data: Buffer): boolean {
  return (
    data.length >= QOIbytes.length &&
    data.slice(0, QOIbytes.length).equals(QOIbytes)
  );
}
