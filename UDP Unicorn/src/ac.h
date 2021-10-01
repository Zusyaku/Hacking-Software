/*
 *  A Win32 UDP flooding utility for penetration testing
 *  Copyright (C) 2011  unixunited@live.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AC_H__
#define __AC_H__

#include "udpunicorn.h"

#define PROTO_TCP       0x01
#define PROTO_UDP       0x02

// UDP table structures
typedef struct _MIB_UDPROW_OWNDER_PID{
    DWORD dwLocalAddr;
    DWORD dwLocalPort;
    DWORD dwOwningPid;
} MIB_UDPROW_OWNER_PID, *PMIB_UDPROW_OWNER_PID;

typedef struct _MIB_UDPTABLE_OWNER_PID{
    DWORD                dwNumEntries;
    MIB_UDPROW_OWNER_PID table[1];
} MIB_UDPTABLE_OWNER_PID, *PMIB_UDPTABLE_OWNER_PID;

typedef struct _MIB_UDP6ROW_OWNER_PID{
    UCHAR ucLocalAddr[16];
    DWORD dwLocalScopeId;
    DWORD dwLocalPort;
    DWORD dwOwningPid;
} MIB_UDP6ROW_OWNER_PID, *PMIB_UDP6ROW_OWNER_PID;

// UDP table enum
typedef enum{
    UDP_TABLE_BASIC,
    UDP_TABLE_OWNER_PID,
    UDP_TABLE_OWNER_MODULE
} UDP_TABLE_CLASS, *PUDP_TABLE_CLASS;

// table data
struct table_t{
    char            local[512];
    char            remote[512];
    unsigned short  loc_port;
    unsigned short  rem_port;
    DWORD           state;
    DWORD           pid;
    DWORD           proto;
};

extern HWND g_hUnicorn;

void        register_ac_class   (void);
VOID WINAPI _ac                 (void);
void        get_udp_table       (void);

#endif
