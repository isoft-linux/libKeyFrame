/*
 * Copyright (C) 2016 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __LIB_KEYFRAME_H__
#define __LIB_KEYFRAME_H__

#define LIBKEYFRAME_EXPORTED __attribute__((__visibility__("default")))

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Find all keyfram from video then save to PNG file.
 *
 * @param src_file      The video path of the keyframe to be extracted
 * @param out_dir       Keyframe output folder
 * @param img_width     Thumbnail width
 * @param img_height    Thumbnail height
 * @return      0 on success, -1 on error
 */
LIBKEYFRAME_EXPORTED int findKeyFrame(char *src_file, 
                                      char *out_dir, 
                                      int img_width, 
                                      int img_height);

#ifdef __cplusplus
}
#endif

#endif /* __LIB_KEYFRAME_H__ */
