/*
 * Copyright(C) 2010,Custom Co., Ltd 
 *    FileName: playback.h
 * Description: 
 *     Version: 1.0
 *      Author: panyadong
 * Create Date: 2010-4-24
 * Modification History£º
 */

#ifndef _PLAYBACK_H_
#define _PLAYBACK_H_


#include "public.h"

int Demo_PlayBack(LONG lUserID);

int Demo_PlayBackByTime(LONG lUserID);
int Demo_GetFileByTime(LONG lUserID);

//Find file
int Demo_FindFile(LONG lUserID);
int Demo_PlayBackByName();

//Find event
int Demo_FindEvent(LONG lUserID);

#endif