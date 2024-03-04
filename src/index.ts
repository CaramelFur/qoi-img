import { Readable, Transform } from 'stream';
import {
  QOIAddonType,
  QOIChannels,
  QOIColorSpace,
  QOIStreamState,
} from './addon';
export * from './addon';

const addon = require('bindings')('qoixx_addon') as QOIAddonType;

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
    data.subarray(0, QOIbytes.length).equals(QOIbytes)
  );
}

export class QOIDecoder extends Transform {
  private state: any;
  private hasEmittedInfo = false;

  constructor(channels?: QOIChannels) {
    super();
    this.state = addon.createDecodeState(channels ?? 0);
  }

  _transform(chunk: Buffer, encoding: string, callback: Function) {
    try {
      const result = addon.streamDecode(this.state, chunk);
      if (!this.hasEmittedInfo) {
        const status = addon.readDecodeState(this.state);
        if (status.state !== QOIStreamState.QOIS_STATE_HEADER) {
          this.emit('info', status);
          this.hasEmittedInfo = true;
        }
      }

      callback(null, result);
    } catch (e) {
      callback(e as Error);
    }
  }

  _final(callback: (error?: Error | null | undefined) => void): void {
    const info = addon.readDecodeState(this.state);
    if (info.state !== QOIStreamState.QOIS_STATE_DONE) {
      callback(new Error('QOI stream ended before decoding was done'));
    }
    callback();
  }

  pipe<T extends NodeJS.WritableStream>(
    destination: T,
    options?: { end?: boolean; sendSharpMetadata?: boolean },
  ): T {
    if (options?.sendSharpMetadata) {
      this.once('info', (info) => {
        const d = destination as any;
        if (!d || !d?.options || !d?.options?.input) return;
        d.options.input.rawWidth = info.width;
        d.options.input.rawHeight = info.height;
        d.options.input.rawChannels = info.channels;
        d.options.input.rawDepth = 'uchar';
        d.options.input.rawPremultiplied = false;
      });
    }

    return super.pipe(destination, options);
  }

  public getInfo() {
    return addon.readDecodeState(this.state);
  }
}

export class QOIEncoder extends Transform {
  private state: any;

  constructor(options?: {
    width: number;
    height: number;
    channels: QOIChannels;
    colorspace?: QOIColorSpace;
  }) {
    super();

    this.state = addon.createEncodeState(
      options?.width ?? 0,
      options?.height ?? 0,
      options?.channels ?? 0 as QOIChannels,
      options?.colorspace ?? QOIColorSpace.SRGB,
    );

    this.once('pipe', this.onPipe.bind(this));
  }

  _transform(chunk: Buffer, encoding: string, callback: Function) {
    try {
      const result = addon.streamEncode(this.state, chunk);
      callback(null, result);
    } catch (e) {
      callback(e as Error);
    }
  }

  _final(callback: (error?: Error | null | undefined) => void): void {
    const info = addon.readEncodeState(this.state);
    if (info.state !== QOIStreamState.QOIS_STATE_DONE) {
      callback(new Error('QOI stream ended before encoding was done'));
    }
    callback();
  }

  private onPipe(src: Readable) {
    src.once(
      'info',
      (info: {
        width: number;
        height: number;
        channels: number;
        colorspace?: number;
      }) => {
        this.state = addon.createEncodeState(
          info.width,
          info.height,
          info.channels,
          !!info.colorspace ? info.colorspace : QOIColorSpace.SRGB,
        );
      },
    );
  }

  public getInfo() {
    return addon.readEncodeState(this.state);
  }
}
