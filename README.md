# qoi-img

Typescript bindings for encoding an decoding the Quite Ok Image format.
Since this uses the [QOIxx](https://github.com/wx257osn2/qoixx) c++ library, instead of javascript, it is much faster than other libraries.
It also uses [qoi-stream](https://github.com/rubikscraft/qoi-stream) for the streaming api, which is written in C. So this library is also very fast.

<https://qoiformat.org/>

## Documentation

### Encoding

You can encode an image to the qoi-img format by using the `QOIEncode` function.
The pixels are ordered as either `RGB` or `RGBA`, depending on what you specify in the channels parameter.

```js
function QOIencode(
  pixels: Buffer,
  options: {
    width: number;
    height: number;
    channels: QOIChannels;
    colorspace?: QOIColorSpace;
  },
) => Buffer;
```

### Decoding

Decoding is just as easy. You can decode an image by using the `QOIDecode` function.
If you leave the convertChannels parameter empty, the channels will automatically be inferred from the image.
However if you set it to either `RGB` or `RGBA`, it will convert the image to the correct channels.

```js
function QOIDecode(data: Buffer, convertChannels?: QOIChannels) => {
  pixels: Buffer;
  width: number;
  height: number;
  channels: QOIChannels;
  colorspace: QOIColorSpace;
}
```

### Stream Encoding

You can also encode a stream of pixels into a qoi image using the streaming api. This is useful if you want to encode a large image without having to load it all into memory.

```js
const input = fs.createReadStream("input.rgb");
const output = fs.createWriteStream("output.qoi");

const encoder = new QOIStreamEncoder({
  width: 64,
  height: 64,
  channels: QOIChannels.RGB,
  colorspace: QOIColorSpace.SRGB,
});

input.pipe(encoder).pipe(output);

const info = encoder.getInfo();

console.log("Width:", info.width);
console.log("Height:", info.height);
console.log("Channels:", info.channels);
console.log("Colorspace:", info.colorspace);
```

### Stream Decoding

The streaming api is also available for decoding qoi images. Here you can also override the channels to either add or remove the alpha channel.

```js
const input = fs.createReadStream("input.qoi");
const output = fs.createWriteStream("output.rgba");

const decoder = new QOIStreamDecoder(QOIChannels.RGBA);

input.pipe(decoder).pipe(output);

const info = decoder.getInfo();

console.log("Width:", info.width);
console.log("Height:", info.height);
console.log("Channels:", info.channels);
console.log("Colorspace:", info.colorspace);
```

## Release notes

* [v2.1.1]
  * Improve documentation
  * Update dependencies
* [v2.1.0]
  * Add sharp integration for streams
* [v2.0.0]
  * Added support for the streaming api
  * Updated qoixx
  * Updated dependencies
* [v1.2.0]
  * Update dependencies
  * Remove building with `-march=native`
* [v1.1.0]
  * Add RGB/RGBA conversion, add tests
* [v1.0.0]
  * Initial release
