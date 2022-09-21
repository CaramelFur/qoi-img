#include <node.h>
#include <node_api.h>

#include "qoixx.hpp"
#include "sqoi.h"

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
  // Third argument is channels
  // Fourth argument is colorspace
  napi_value CreateStreamDescription(napi_env env, napi_callback_info args)
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

    qoi_desc_t *desc = (qoi_desc_t *)malloc(sizeof(qoi_desc_t));
    desc->width = width;
    desc->height = height;
    desc->channels = channels;
    desc->colorspace = colorspace;

    napi_value result;
    status = napi_create_external(env, desc, finalize_data, nullptr, &result);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Failed to create result");
      return nullptr;
    }

    return result;
  }

  // No arguments
  napi_value CreateEncodeState(napi_env env, napi_callback_info args)
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

    qoi_enc_t *state = (qoi_enc_t *)malloc(sizeof(qoi_enc_t));

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

    qoi_dec_t *state = (qoi_dec_t *)malloc(sizeof(qoi_dec_t));

    napi_value result;
    status = napi_create_external(env, state, finalize_data, nullptr, &result);

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
    napi_value encode;
    napi_value decode;
    napi_value create_stream_description;
    napi_value create_encode_state;
    napi_value create_decode_state;

    status = napi_create_function(env, nullptr, 0, Encode, nullptr, &encode);
    if (status != napi_ok)
      return nullptr;

    status = napi_set_named_property(env, exports, "encode", encode);
    if (status != napi_ok)
      return nullptr;

    status = napi_create_function(env, nullptr, 0, Decode, nullptr, &decode);
    if (status != napi_ok)
      return nullptr;

    status = napi_set_named_property(env, exports, "decode", decode);
    if (status != napi_ok)
      return nullptr;

    status = napi_create_function(env, nullptr, 0, CreateStreamDescription, nullptr, &create_stream_description);
    if (status != napi_ok)
      return nullptr;

    status = napi_set_named_property(env, exports, "create_stream_description", create_stream_description);
    if (status != napi_ok)
      return nullptr;

    status = napi_create_function(env, nullptr, 0, CreateEncodeState, nullptr, &create_encode_state);
    if (status != napi_ok)
      return nullptr;

    status = napi_set_named_property(env, exports, "create_encode_state", create_encode_state);
    if (status != napi_ok)
      return nullptr;

    status = napi_create_function(env, nullptr, 0, CreateDecodeState, nullptr, &create_decode_state);
    if (status != napi_ok)
      return nullptr;

    status = napi_set_named_property(env, exports, "create_decode_state", create_decode_state);
    if (status != napi_ok)
      return nullptr;

    return exports;
  }

  NAPI_MODULE(NODE_GYP_MODULE_NAME, init)
}
