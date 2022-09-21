/* After reading a QOI file and placed in buffer */
#include "./lib/sqoi.h"

qoi_desc_t desc;
qoi_dec_t dec;
qoi_pixel_t px;

uint8_t *bytes;
size_t rawImageLength, seek;

qoi_desc_init(&desc);
qoi_initalize_pixel(&px);
qoi_set_pixel_rgba(&px, 0, 0, 0, 255);

if (!read_qoi_header(&desc, qoi_bytes))
{
	printf("The file you opened is not a QOIF file\n");
	return;
}

raw_image_length = (size_t)desc.width * (size_t)desc.height * (size_t)desc.channels;

seek = 0;
if (raw_image_length == 0)
	return;

qoi_dec_init(&desc, &dec, qoi_bytes, buffer_size);
/* Creates a blank image for the decoder to work on */
bytes = (unsigned char*)malloc(raw_image_length * sizeof(unsigned char) + 4);

if (!bytes)
	return;

/* Keep decoding the pixels until
all pixels are done decompressing */
while (!qoi_dec_done(&dec))
{
	px = qoi_decode_chunk(&dec);
	/* Do something with the pixel values below */
	bytes[seek] = px.red;
	bytes[seek + 1] = px.green;
	bytes[seek + 2] = px.blue;
	
	if (desc.channels > 3) 
		bytes[seek + 3] = px.alpha;
	
	seek += desc.channels;
}

/* Use the pixels however you want after this code */
