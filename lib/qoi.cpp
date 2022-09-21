#include <node.h>
#include <node_api.h>

#include "qoixx.hpp"
#include "sqoi.h"

#define NAPI_EXPORT(function, name) \
  napi_value name; \
  status = napi_create_function(env, nullptr, 0, function, nullptr, &name); \
  if (status != napi_ok) \
    return nullptr; \
  \
  status = napi_set_named_property(env, exports, #name, name); \
  if (status != napi_ok) \
    return nullptr; \

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
  napi_value WriteQoiHeader(napi_env env, napi_callback_info args)
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

    qoi_desc_t desc;
    desc.width = width;
    desc.height = height;
    desc.channels = channels;
    desc.colorspace = colorspace;

    uint8_t target[14] = {0};
    size_t target_length = sizeof(target);

    write_qoi_header(&desc, target, target_length);

    napi_value result;
    status = napi_create_buffer_copy(env, target_length, (char *)target, nullptr, &result);

    if (status != napi_ok)
    {
      napi_throw_error(env, nullptr, "Failed to create buffer");
      return nullptr;
    }

    return result;
  }

  // First argument is a buffer with minimum size of 14
  napi_value ReadQoiHeader(napi_env env, napi_callback_info args)
  {
    uint32_t status;

    napi_value argv[1];
    size_t argc = 1;
    status = napi_get_cb_info(env, args, &argc, argv, nullptr, nullptr);

    if (status != napi_ok || argc != 1)
    {
      napi_throw_error(env, nullptr, "Invalid argument count");
      return nullptr;
    }

    size_t buffer_size = 0;
    uint8_t *buffer = nullptr;
    status = napi_get_buffer_info(env, argv[0], (void **)&buffer, &buffer_size);

    if (status != napi_ok || buffer_size < 14)
    {
      napi_throw_error(env, nullptr, "Invalid argument type");
      return nullptr;
    }

    qoi_desc_t desc;
    qoi_desc_init(&desc);

    if (!read_qoi_header(&desc, buffer, buffer_size))
    {
      napi_throw_error(env, nullptr, "Invalid QOI header");
      return nullptr;
    }

    napi_value result_desc;
    status = napi_create_object(env, &result_desc);

    napi_value result_width;
    napi_value result_height;
    napi_value result_channels;
    napi_value result_colorspace;

    status += napi_create_uint32(env, desc.width, &result_width);
    status += napi_create_uint32(env, desc.height, &result_height);
    status += napi_create_uint32(env, desc.channels, &result_channels);
    status += napi_create_uint32(env, desc.colorspace, &result_colorspace);

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

  // First argument is a decode state external
  // 
  napi_value StreamDecode(napi_env env, napi_callback_info args) {

  } 

  napi_value init(napi_env env, napi_value exports)
  {
    napi_status status;
    
    NAPI_EXPORT(Encode, encode);
    NAPI_EXPORT(Decode, decode);
    NAPI_EXPORT(CreateEncodeState, createEncodeState);
    NAPI_EXPORT(CreateDecodeState, createDecodeState);
    NAPI_EXPORT(ReadQoiHeader, readQoiHeader);
    NAPI_EXPORT(WriteQoiHeader, writeQoiHeader);

    return exports;
  }

  NAPI_MODULE(NODE_GYP_MODULE_NAME, init)
}
