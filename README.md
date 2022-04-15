# qoi-img

Typescript bindings for encoding an decoding the Quite Ok Image format.
Since this uses the [QOIxx](https://github.com/wx257osn2/qoixx) c++ library, instead of javascript, it is much faster than other libraries.

<https://qoiformat.org/>

## Documentation

### Encoding

You can encode an image to the qoi-img format by using the `QOIEncode` function.
The pixels are ordered as either `RGB` or `RGBA`, depending on what you specify in the channels parameter.

```js
function QOIEncode: (
    pixels: Buffer,
    width: number,
    height: number,
    channels: QOIChannels,
    colorSpace: QOIColorSpace,
  ) => Buffer;
```

### Decoding

Decoding is just as easy. You can decode an image by using the `QOIDecode` function.

```js
function QOIDecode(data: Buffer) => {
    pixels: Buffer;
    width: number;
    height: number;
    channels: QOIChannels;
    colorSpace: QOIColorSpace;
  }
```

### Enums

```js
export enum QOIColorSpace {
  SRGB = 0,
  Linear = 1,
}

export enum QOIChannels {
  RGB = 3,
  RGBA = 4,
}
```
