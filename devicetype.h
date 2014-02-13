#ifndef DEVICE_TYPE_H
#define DEVICE_TYPE_H

#define DEVICE_DH               3U
#define DEVICE_XM               34U

#define GET_ENCODE_CONFIG	                                    1

#define VIDEO_STREAM_DATA			0x0000000000000001
#define AUDIO_STREAM_DATA			0x0000000000000002
#define ALARM_STREAM_DATA			0x0000000000000004

#define VIDEO_ENCODE_UNKNOW                                 0x0008
#define VIDEO_ENCODE_JPEG                                   0x0010
#define VIDEO_ENCODE_HIK_JPEG                               0x0011
#define VIDEO_ENCODE_DAHUA_JPEG                             0x0012

#define VIDEO_ENCODE_VIDEO_MPEG4                            0x0020
#define VIDEO_ENCODE_HIK_MPEG4                              0x0021
#define VIDEO_ENCODE_DAHUA_MPEG4                            0x0022

#define VIDEO_ENCODE_VIDEO_MPEG2                            0x0040
#define VIDEO_ENCODE_HIK_MPEG2                              0x0041

#define VIDEO_ENCODE_VIDEO_H264                             0x0080
#define VIDEO_ENCODE_HIK_H264                               0x0081
#define VIDEO_ENCODE_DAHUA_H264                             0x0082

#define UNKNOWN_FRAME                                        0
#define I_FRAME                                              1
#define B_FRAME                                              2
#define P_FRAME                                              3

#define AUDIO_G711                                        1
#define AUDIO_G711A                                       10

enum {JPTZ_UP=0, JPTZ_RIGHT_UP, JPTZ_RIGHT, JPTZ_RIGHT_DOWN, 
	  JPTZ_DOWN, JPTZ_LEFT_DOWN, JPTZ_LEFT, JPTZ_LEFT_UP, 
	  JPTZ_STOP};

#endif
