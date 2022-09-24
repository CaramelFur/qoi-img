#include <node.h>
#include <node_api.h>
#include <vector>

#include "qoixx.hpp"
#include "qoi-stream.h"

#define NAPI_EXPORT(function, name)                                         \
  napi_value name;                                                          \
  status = napi_create_function(env, nullptr, 0, function, nullptr, &name); \
  if (status != napi_ok)                                                    \
    return nullptr;                                                         \
                                                                            \
  status = napi_set_named_property(env, exports, #name, name);              \
  if (status != napi_ok)                                                    \
    return nullptr;

namespace qoi_node
{
  static void finalize_data(napi_env env, void *data, void *hint)
  {
    free(data);
  }

  // First argument is a Buffer
  // Second argument is width
  // Third argument is height
  // Fourth argument is channels
  // Fifth argument is colorspace
  napi_value Encode(napi_env env, napi_callback_info args)
  {
    uint32_t status;

    napi_value argv[5];
    size_t argc = 5;
    status = napi_get_cb_info(env, args, &argc, argv, nullptr, nullptr);

    if (status != napi_ok || argc != 5)
    {
      napi_throw_error(env, nullptr, "Invalid argument count");
      return nullptr;
    }

    uint8_t *pixels;
    size_t pixels_length;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t channels = 0;
    uint32_t colorspace = 0;

    status = napi_get_buffer_info(env, argv[0], (void **)&pixels, &pixels_length);
    status += napi_get_value_uint32(env, argv[1], &width);
    status += napi_get_value_uint32(env, argv[2], &height);
    status += napi_get_value_uint32(env, argv[3], &channels);
    status += napi_get_value_uint32(env, argv[4], &colorspace);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Invalid argument type");
      return nullptr;
    }

    const auto options = qoixx::qoi::desc{
        .width = width,
        .height = height,
        .channels = (uint8_t)channels,
        .colorspace = (qoixx::qoi::colorspace)colorspace,
    };

    std::vector<std::byte> image;
    try
    {
      image = qoixx::qoi::encode<std::vector<std::byte>>(pixels, pixels_length, options);
    }
    catch (const std::exception &e)
    {
      napi_throw_error(env, nullptr, "Failed to encode image");
      return nullptr;
    }

    char *image_pixels = reinterpret_cast<char *>(image.data());
    std::size_t image_length = image.size();

    napi_value result;
    status = napi_create_buffer_copy(env, image_length, image_pixels, nullptr, &result);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Failed to create buffer");
      return nullptr;
    }

    return result;
  }

  // First argument is a Buffer
  // Second argument is channels override (0, 3, 4)
  napi_value Decode(napi_env env, napi_callback_info args)
  {
    uint32_t status;

    napi_value argv[2];
    size_t argc = 2;
    status = napi_get_cb_info(env, args, &argc, argv, nullptr, nullptr);

    if (status != napi_ok || argc != 2)
    {
      napi_throw_error(env, nullptr, "Invalid argument count");
      return nullptr;
    }

    uint8_t *pixels;
    size_t pixels_length;
    uint32_t channels = 0;

    status = napi_get_buffer_info(env, argv[0], (void **)&pixels, &pixels_length);
    status += napi_get_value_uint32(env, argv[1], &channels);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Invalid argument type");
      return nullptr;
    }

    std::pair<std::vector<std::byte>, qoixx::qoi::desc> image;
    try
    {
      image = qoixx::qoi::decode<std::vector<std::byte>>(pixels, pixels_length, (uint8_t)channels);
    }
    catch (const std::exception &e)
    {
      napi_throw_error(env, nullptr, "Failed to decode image");
      return nullptr;
    }

    char *image_pixels = reinterpret_cast<char *>(image.first.data());
    size_t image_length = image.first.size();

    napi_value result_pixels;
    napi_value result_width;
    napi_value result_height;
    napi_value result_channels;
    napi_value result_colorspace;

    status = napi_create_buffer_copy(env, image_length, image_pixels, nullptr, &result_pixels);
    status += napi_create_uint32(env, image.second.width, &result_width);
    status += napi_create_uint32(env, image.second.height, &result_height);
    status += napi_create_uint32(env, (uint32_t)image.second.channels, &result_channels);
    status += napi_create_uint32(env, (uint32_t)image.second.colorspace, &result_colorspace);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Failed to create result");
      return nullptr;
    }

    napi_value result_desc;
    status = napi_create_object(env, &result_desc);
    status += napi_set_named_property(env, result_desc, "pixels", result_pixels);
    status += napi_set_named_property(env, result_desc, "width", result_width);
    status += napi_set_named_property(env, result_desc, "height", result_height);
    status += napi_set_named_property(env, result_desc, "channels", result_channels);
    status += napi_set_named_property(env, result_desc, "colorspace", result_colorspace);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Failed to create result");
      return nullptr;
    }

    return result_desc;
  }

  // First argument is width
  // Second argument is height
  // Third argument is channels (3, 4)
  // Fourth argument is colorspace (0, 1)
  napi_value CreateEncodeState(napi_env env, napi_callback_info args)
  {
    // modify every byte in the stream by adding 2 to it
    uint32_t status;

    napi_value argv[4];
    size_t argc = 4;
    status = napi_get_cb_info(env, args, &argc, argv, nullptr, nullptr);

    if (status != napi_ok || argc != 4)
    {
      napi_throw_error(env, nullptr, "Invalid argument count");
      return nullptr;
    }

    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t channels = 0;
    uint32_t colorspace = 0;

    status += napi_get_value_uint32(env, argv[0], &width);
    status += napi_get_value_uint32(env, argv[1], &height);
    status += napi_get_value_uint32(env, argv[2], &channels);
    status += napi_get_value_uint32(env, argv[3], &colorspace);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Invalid argument type");
      return nullptr;
    }

    qois_enc_state *state = (qois_enc_state *)malloc(sizeof(qois_enc_state));
    qois_enc_state_init(state, width, height, (uint8_t)channels, (uint8_t)colorspace);

    napi_value result;
    status = napi_create_external(env, state, finalize_data, nullptr, &result);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Failed to create result");
      return nullptr;
    }

    return result;
  }

  // No Arguments
  napi_value CreateDecodeState(napi_env env, napi_callback_info args)
  {
    // modify every byte in the stream by adding 2 to it
    uint32_t status;

    size_t argc = 0;
    status = napi_get_cb_info(env, args, &argc, nullptr, nullptr, nullptr);

    if (status != napi_ok || argc != 0)
    {
      napi_throw_error(env, nullptr, "Invalid argument count");
      return nullptr;
    }

    qois_dec_state *state = (qois_dec_state *)malloc(sizeof(qois_dec_state));
    qois_dec_state_init(state);

    napi_value result;
    status = napi_create_external(env, state, finalize_data, nullptr, &result);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Failed to create result");
      return nullptr;
    }

    return result;
  }

  // First argument is a decode state external
  // Second argument is a buffer of pixels
  napi_value StreamDecode(napi_env env, napi_callback_info args)
  {
    // modify every byte in the stream by adding 2 to it
    uint32_t status;

    napi_value argv[2];
    size_t argc = 2;
    status = napi_get_cb_info(env, args, &argc, argv, nullptr, nullptr);

    if (status != napi_ok || argc != 2)
    {
      napi_throw_error(env, nullptr, "Invalid argument count");
      return nullptr;
    }

    qois_dec_state *state;
    uint8_t *bytes;
    size_t bytes_length;

    status += napi_get_value_external(env, argv[0], (void **)(&state));
    status += napi_get_buffer_info(env, argv[1], (void **)(&bytes), &bytes_length);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Invalid argument type");
      return nullptr;
    }

    // create vector
    std::vector<uint8_t> pixels(bytes_length * 4);
    size_t position = 0;

    for (size_t i = 0; i < bytes_length; i++)
    {
      size_t remaining = pixels.size() - position;
      int written = qois_decode_byte(state, bytes[i], pixels.data() + position, remaining);
      if (written < 0)
      {
        napi_throw_error(env, nullptr, "Failed to decode");
        return nullptr;
      }
      position += written;

      if (remaining < 0xff) {
        pixels.resize(pixels.size() + (bytes_length - i) * 4);
      }
    }
    
    napi_value result;
    status = napi_create_buffer_copy(env, position, pixels.data(), nullptr, &result);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Failed to create result");
      return nullptr;
    }

    return result;
  }

  // First argument is an encode state external
  // Second argument is a buffer of pixels
  napi_value StreamEncode(napi_env env, napi_callback_info args)
  {
    // modify every byte in the stream by adding 2 to it
    uint32_t status;

    napi_value argv[2];
    size_t argc = 2;
    status = napi_get_cb_info(env, args, &argc, argv, nullptr, nullptr);

    if (status != napi_ok || argc != 2)
    {
      napi_throw_error(env, nullptr, "Invalid argument count");
      return nullptr;
    }

    qois_enc_state *state;
    uint8_t *bytes;
    size_t bytes_length;

    status += napi_get_value_external(env, argv[0], (void **)(&state));
    status += napi_get_buffer_info(env, argv[1], (void **)(&bytes), &bytes_length);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Invalid argument type");
      return nullptr;
    }

    // create vector
    std::vector<uint8_t> pixels(bytes_length);
    size_t position = 0;

    for (size_t i = 0; i < bytes_length; i++)
    {
      size_t remaining = pixels.size() - position;
      int written = qois_encode_byte(state, bytes[i], pixels.data() + position, remaining);
      if (written < 0)
      {
        napi_throw_error(env, nullptr, "Failed to encode");
        return nullptr;
      }
      position += written;

      if (remaining < 0xff) {
        pixels.resize(pixels.size() + bytes_length - 1);
      }
    }
    
    napi_value result;
    status = napi_create_buffer_copy(env, position, pixels.data(), nullptr, &result);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Failed to create result");
      return nullptr;
    }

    return result;
  }

  napi_value init(napi_env env, napi_value exports)
  {
    napi_status status;

    NAPI_EXPORT(Encode, encode);
    NAPI_EXPORT(Decode, decode);
    NAPI_EXPORT(CreateEncodeState, createEncodeState);
    NAPI_EXPORT(CreateDecodeState, createDecodeState);
    NAPI_EXPORT(StreamDecode, streamDecode);
    NAPI_EXPORT(StreamEncode, streamEncode);

    return exports;
  }

  NAPI_MODULE(NODE_GYP_MODULE_NAME, init)
}
