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

#define UNICORN_FONT        "Segoe UI"

#define DLG_MAIN            101
#define DLG_ABOUT           102
#define DLG_FILTER          103
#define DLG_NETINFO         104
#define DLG_PS_OP           105

#define IDR_MENU            110
#define IDR_FILE_RESET      111
#define IDR_FILE_QUIT       112
#define IDR_HELP_ABOUT      113
#define IDR_HELP_FAQ        114
#define IDR_OP_BGCOLOR      115
#define IDR_TOOLS_CONNECT   116
#define IDR_TOOLS_PORTLIST  117
#define IDR_TOOLS_NETINFO   118
#define IDR_TOOLS_PORTSCAN  119
#define IDR_OP_TOP          120
#define IDR_OP_PORTSCAN     121
#define IDR_OP_RESETBG      122
#define IDR_TRAYICON        123
#define IDR_TRAY_SHOW       124
#define IDR_TRAY_CLOSE      125
#define IDR_OP_MIN_TRAY     126
#define IDR_OP_CONFIG       127


#define IDI_MAIN            201
#define IDI_IDLE            202
#define IDI_ATTACKING       203
#define IDI_MUSIC           204

// controls
#define IDC_EDT_TARGET      901
#define IDC_EDT_PORT        902
#define IDC_TB_SIZE         903
#define IDC_EDT_SIZE        904
#define IDC_EDT_DELAY       905
#define IDC_EDT_THREADS     906
#define IDC_EDT_SOCKETS     907
#define IDC_BTN_TEST        908
#define IDC_EDT_TEST        909
#define IDC_ICO_LEFTI       910
#define IDC_ICO_RIGHTI      911
#define IDC_ICO_LEFTA       912
#define IDC_ICO_RIGHTA      913
#define IDC_BTN_ATTACK      914
#define IDC_EDT_DATASENT    915
#define IDC_BTN_DNS         916
#define IDC_EDT_DNS         917
#define IDC_BTN_RESETDATA   918
#define IDC_CHK_RANDSIZE    919
#define IDC_BTN_MUSIC       920

//---dialogs---
// active connections
#define WC_AC               "|__UDP_UNICORN_ACTIVE_CONNECTIONS__|"
#define IDC_AC_LST          930
#define IDR_AC_REFRESH      931
#define IDR_AC_SET_TARGET   932
#define IDR_AC_FILTER_PORT  933
#define IDR_AC_SENDTO_PS    934

// filter port
#define IDC_FILTER_EDT_PORT 940

// port scanner
#define WC_PS               "|__UDP_UNICORN_PORT_SCANNER__|"
#define IDC_PS_EDT_HOST     950
#define IDC_PS_LST          951
#define IDC_PS_PROGRESS     952

// net info
#define IDC_NETINFO_EDT1    960
#define IDC_NETINFO_EDT2    961
#define IDC_NETINFO_EDT3    962
#define IDC_NETINFO_EDT4    963

// port scanner options
#define IDC_PS_OP_EDT1      970
#define IDC_PS_OP_EDT2      971

// about
#define IDC_ABOUT_LINK      1021
