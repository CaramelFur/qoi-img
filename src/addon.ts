export enum QOIColorSpace {
  SRGB = 0,
  Linear = 1,
}

export enum QOIChannels {
  RGB = 3,
  RGBA = 4,
}

export enum QOIStreamState {
  QOIS_STATE_HEADER = 0,
  QOIS_STATE_FOOTER,
  QOIS_STATE_DONE,

  QOIS_OP_NONE = 10,
  QOIS_OP_RGB,
  QOIS_OP_RGBA,

  QOIS_OP_INDEX,
  QOIS_OP_DIFF,
  QOIS_OP_LUMA,
  QOIS_OP_RUN,
}

export interface QOIStreamInfo {
  state: QOIStreamState;
  width: number;
  height: number;
  channels: QOIChannels;
  colorspace: QOIColorSpace;
}

export type QOIAddonType = {
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
  createEncodeState: (
    width: number,
    height: number,
    channels: QOIChannels,
    colorspace: QOIColorSpace,
  ) => any;
  createDecodeState: (
    channels: QOIChannels | 0,
  ) => any;
  readEncodeState: (state: any) => QOIStreamInfo;
  readDecodeState: (state: any) => QOIStreamInfo;
  streamEncode: (state: any, chunk: Buffer) => Buffer;
  streamDecode: (state: any, chunk: Buffer) => Buffer;
};
