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
    colorspace: QOIColorSpace,
  ) => Buffer;
```

### Decoding

Decoding is just as easy. You can decode an image by using the `QOIDecode` function.
You can also convert the image to either RGB or RGBA by setting the convertChannels parameter.

```js
function QOIDecode(data: Buffer, convertChannels?: QOIChannels) => {
    pixels: Buffer;
    width: number;
    height: number;
    channels: QOIChannels;
    colorspace: QOIColorSpace;
  }
```

### Enums

```js
enum QOIColorSpace {
  SRGB = 0,
  Linear = 1,
}

enum QOIChannels {
  RGB = 3,
  RGBA = 4,
}
```

## Release notes

* [v1.1.0]
  * Add RGB/RGBA conversion, add tests
* [v1.0.0]
  * Initial release
