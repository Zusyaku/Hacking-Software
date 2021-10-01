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

#ifndef UDPUNICORN_H
#define UDPUNICORN_H

//#define DEBUG

#define STRICT
#define _WIN32_IE     0x501
#define _WIN32_WINNT  0x501
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>
#include <commdlg.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>

#include "resource.h"

#define RAND_PORT 777
#define RAND_SIZE 666

// protocol definitions
#define SSH         22
#define TELNET      23
#define SMTP        25
#define DNS         53
#define DHCP        67
#define HTTP        80
#define KERBEROS    88
#define NETBIOS     137
#define IMAP4       143
#define SNMP        161
#define LDAP        389
#define HTTPS       443
#define RIP         550
#define SFTP_SSL    990
#define MS_SQL      1433
#define MSN         1863
#define MYSQL       3306
#define ITUNES      3689
#define VENT        3784
#define ICQ         4000
#define AOL         5190
#define XFIRE       25777

// games
#define XBOX_LIVE   3074
#define PSN         3478
#define COD         28960
#define MW2         27014
#define CSTRIKE     27020
#define MC          25565
#define TF2         27015
#define WOW         3724
#define LOL         5000

// protocol string definitions
#define _SSH        "SSH"
#define _TELNET     "Telnet"
#define _SMTP       "SMTP"
#define _DNS        "DNS"
#define _DHCP       "DHCP"
#define _HTTP       "HTTP"
#define _KERBEROS   "Kerberos"
#define _NETBIOS    "Netbios"
#define _IMAP4      "IMAP4"
#define _SNMP       "SNMP"
#define _LDAP       "LDAP"
#define _HTTPS      "HTTPS"
#define _RIP        "RIP"
#define _SFTP_SSL   "SFTP(SSL)"
#define _MS_SQL     "Microsoft SQL"
#define _MSN        "MSN"
#define _MYSQL      "MySQL"
#define _ITUNES     "iTunes"
#define _VENT       "Ventrilo"
#define _ICQ        "ICQ"
#define _AOL        "AOL"
#define _XFIRE      "Xfire Chat"

// game strings
#define _XBOX_LIVE  "Xbox LIVE"
#define _PSN        "PSN (PS3)"
#define _COD        "Call of Duty"
#define _MW2        "Modern Warfare 2"
#define _CSTRIKE    "Counter Strike"
#define _MC         "Minecraft"
#define _TF2        "Team Fortress 2"
#define _WOW        "World of Warcraft"
#define _LOL        "League of Legends"

#define TEMP_FILE "UDP_Unicorn_temp.txt"

#define VER_LONGHORN    0x01
#define VER_XP          0x02
#define VER_NT          0x03

#define MIN_HIDE        1
#define MIN_SHOW        0

//------------------------------

// structures required to use functions in ICMP.DLL
typedef struct {
    unsigned char Ttl;                         // Time To Live
    unsigned char Tos;                         // Type Of Service
    unsigned char Flags;                       // IP header flags
    unsigned char OptionsSize;                 // Size in bytes of options data
    unsigned char *OptionsData;                // Pointer to options data
} IP_OPTION_INFORMATION, *PIP_OPTION_INFORMATION;

typedef struct {
    DWORD Address;                             // Replying address
    unsigned long  Status;                     // Reply status
    unsigned long  RoundTripTime;              // RTT in milliseconds
    unsigned short DataSize;                   // Echo data size
    unsigned short Reserved;                   // Reserved for system use
    void *Data;                                // Pointer to the echo data
    IP_OPTION_INFORMATION Options;             // Reply options
} IP_ECHO_REPLY, *PIP_ECHO_REPLY;

typedef struct ip {
   unsigned int   ip_hl; /* both fields are 4 bits */
   unsigned int   ip_v;
   uint8_t        ip_tos;
   uint16_t       ip_len;
   uint16_t       ip_id;
   uint16_t       ip_off;
   uint8_t        ip_ttl;
   uint8_t        ip_p;
   uint16_t       ip_sum;
   struct in_addr ip_src;
   struct in_addr ip_dst;
} IP_HDR, *PIP_HDR;

typedef struct udphdr {
    u_short uh_sport; /* src port */
    u_short uh_dport; /* dst port */
    short uh_ulen; /* udp length */
    u_short uh_sum; /* checksum */
} UDP_HDR, *PUDP_HDR;


// structure to hold target data
typedef struct data_t
{
    char ip[256];
    char src_ip[256];
    unsigned int port;
    size_t size;
    unsigned int delay;
    unsigned int sockets;
} DATA, *PDATA;

//------------------------------

inline  int err         (const char* msg);
int     get_os_ver      (void);
BOOL    resolve_ip      (char* hostname);
BOOL    start_winsock   (void);
void    ping            (HWND hwnd);
void    dns             (HWND hwnd);
void    setup_attack    (HWND hwnd);

#endif

