/*
 * H.264 Plugin codec for OpenH323/OPAL
 *
 * Copyright (C) 2007 Matthias Schneider, All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#ifndef __X264LOADER_H__
#define __X264LOADER_H__ 1

#include <stdint.h>    

extern "C" {
#include <x264.h>
};

typedef void (*Function)();

class X264Library
{
  public:
    X264Library();
    ~X264Library();

    bool Load();
    bool isLoaded() const { return _isLoaded; }
    bool GetFunction(const char * name, Function & func);
    

    x264_t *(*Xx264_encoder_open)(x264_param_t *);
    void (*Xx264_param_default)(x264_param_t *);
    int (*Xx264_encoder_encode)( x264_t *, x264_nal_t **, int *, x264_picture_t *, x264_picture_t *);
    int (*Xx264_nal_encode)(void *, x264_nal_t *nal, int b_annexb, int);
    int (*Xx264_encoder_reconfig)(x264_t *, x264_param_t *);
    int (*Xx264_encoder_headers)( x264_t *, x264_nal_t **, int *);
    void (*Xx264_encoder_close)( x264_t *);
    void (*Xx264_picture_alloc)( x264_picture_t *pic, int i_csp, int i_width, int i_height);
    void (*Xx264_picture_clean)( x264_picture_t *pic);
    int (*Xx264_param_default_preset)( x264_param_t *param, const char *preset, const char *tune );
    int (*Xx264_param_apply_profile)( x264_param_t *param, const char *profile );
    int (*Xx264_param_parse)( x264_param_t *p, const char *name, const char *value );
    int (*Xx264_encoder_delayed_frames)( x264_t *);

  protected:
    bool Open(const char *name);

    bool _isLoaded;
    void * _dynamicLibrary;

};
#endif /*__X264LOADER_H__ 1*/
