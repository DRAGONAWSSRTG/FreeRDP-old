/*
   FreeRDP: A Remote Desktop Protocol client.
   RemoteFX Codec Library - Decode

   Copyright 2011 Vic Lee

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rfx_rlgr.h"
#include "rfx_differential.h"
#include "rfx_quantization.h"
#include "rfx_dwt.h"

#include "rfx_decode.h"

#define MINMAX(_v,_l,_h) ((_v) < (_l) ? (_l) : ((_v) > (_h) ? (_h) : (_v)))

void
rfx_encode_RGB_to_YCbCr(uint32 * y_r_buf, uint32 * cb_g_buf, uint32 * cr_b_buf)
{
	int y, cb, cr;
	int r, g, b;

	int i;
	for (i = 0; i < 4096; i++)
	{
		r = y_r_buf[i];
		g = cb_g_buf[i];
		b = cr_b_buf[i];
		y = ((r >> 2) + (r >> 5) + (r >> 6)) + ((g >> 1) + (g >> 4) + (g >> 6) + (g >> 7)) + ((b >> 4) + (b >> 5) + (b >> 6));
		y_r_buf[i] = MINMAX(y, 0, 255) - 128;
		cb = 0 - ((r >> 3) + (r >> 5) + (r >> 7)) - ((g >> 2) + (g >> 4) + (g >> 6)) + (b >> 1);
		cb_g_buf[i] = MINMAX(cb, -128, 127);
		cr = (r >> 1) - ((g >> 2) + (g >> 3) + (g >> 5) + (g >> 7)) - ((b >> 4) + (b >> 6));
		cr_b_buf[i] = MINMAX(cr, -128, 127);
	}
}

void
rfx_encode_rgb(RFX_CONTEXT * context, const uint8 * rgb_buffer, int rowstride)
{
	int x, y;
	const uint8 * src;
	uint32 * y_r_buffer = context->y_r_buffer;
	uint32 * cb_g_buffer = context->cb_g_buffer;
	uint32 * cr_b_buffer = context->cr_b_buffer;

	for (y = 0; y < 64; y++)
	{
		src = rgb_buffer + y * rowstride;

		for (x = 0; x < 64; x++)
		{
			switch (context->pixel_format)
			{
				case RFX_PIXEL_FORMAT_BGRA:
					*cr_b_buffer++ = (uint32) (*src++);
					*cb_g_buffer++ = (uint32) (*src++);
					*y_r_buffer++ = (uint32) (*src++);
					src++;
					break;
				case RFX_PIXEL_FORMAT_RGBA:
					*y_r_buffer++ = (uint32) (*src++);
					*cb_g_buffer++ = (uint32) (*src++);
					*cr_b_buffer++ = (uint32) (*src++);
					src++;
					break;
				case RFX_PIXEL_FORMAT_BGR:
					*cr_b_buffer++ = (uint32) (*src++);
					*cb_g_buffer++ = (uint32) (*src++);
					*y_r_buffer++ = (uint32) (*src++);
					break;
				case RFX_PIXEL_FORMAT_RGB:
					*y_r_buffer++ = (uint32) (*src++);
					*cb_g_buffer++ = (uint32) (*src++);
					*cr_b_buffer++ = (uint32) (*src++);
					break;
				default:
					break;
			}
		}
	}

	context->encode_RGB_to_YCbCr(context->y_r_buffer, context->cb_g_buffer, context->cr_b_buffer);
}
