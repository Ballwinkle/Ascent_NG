/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2011 Ascent Team <http://www.ascentemulator.net/>
 *
 * This software is  under the terms of the EULA License
 * All title, including but not limited to copyrights, in and to the Ascent Software
 * and any copies there of are owned by ZEDCLANS INC. or its suppliers. All title
 * and intellectual property rights in and to the content which may be accessed through
 * use of the Ascent is the property of the respective content owner and may be protected
 * by applicable copyright or other intellectual property laws and treaties. This EULA grants
 * you no rights to use such content. All rights not expressly granted are reserved by ZEDCLANS INC.
 *
 */

#ifndef __LOG_H
#define __LOG_H

// logging functions
// level 0 = notice/error
// level 1 = information
// level 2 = debug

enum LOGLEVEL
{
#ifdef WIN32
#undef ERROR
#endif
	ERROR		= 0,
	NOTICE		= 0,
	WARNING		= 0,
	INFO		= 1,
	DEBUG		= 2,
};

void log_setloglevel(int new_level);
void log_setlogfile(const char * filename);
void log_write(int level, const char * format, ...);
void log_open();
void log_close();

#endif
