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

#include "ps.h"
#include "ac.h"

BOOL g_bScanning;
char* g_lpHostname;
unsigned int g_nPorts;
int g_nPortStart = 1,
    g_nPortEnd = 1024,
    g_nConnections,
    g_nMaxConnections = MAX_CON;

//////////////////////////////////////////////////////////////////////////////

/* attempt to connect to a server on a certain port */
static DWORD WINAPI _tcp_scan(LPVOID lpBuffer)
{
     HWND   hList           = GetDlgItem(FindWindow(WC_PS, NULL), IDC_PS_LST);
     struct scan_t* scan    = (struct scan_t*)lpBuffer;
     HANDLE hEvent          = NULL;
     SOCKET sock            = INVALID_SOCKET;
     struct sockaddr_in addr;

     // setup target address information
     addr.sin_family        = AF_INET;
     addr.sin_addr.s_addr   = inet_addr(g_lpHostname);
     addr.sin_port          = htons(scan->port);

     // setup TCP socket
     sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     if(sock == INVALID_SOCKET)
     {
         free(scan);
         ExitThread(0);
     }

     // set the event, for timeout
     hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
     WSAEventSelect(sock, hEvent, FD_CONNECT);

     connect(sock, (struct sockaddr*)&addr, sizeof(addr));

     // wait for 2 seconds on a single port
     if(WaitForSingleObject(hEvent, 2000) == WAIT_OBJECT_0)
     {
         WSANETWORKEVENTS ne;

         WSAEnumNetworkEvents(sock, NULL, &ne);
         if(ne.iErrorCode[FD_CONNECT_BIT] == 0)
         {
             char buf[512] = {0};

             // print the open port
             if(g_bScanning)
             {
                 wsprintf(buf, "Found open port %d on %s", scan->port, g_lpHostname);
                 SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)buf);
                 ++g_nPorts;
             }
         }
     }

     closesocket(sock);
     --g_nConnections;

     WSAEventSelect(sock, hEvent, 0);
     CloseHandle(hEvent);

     free(scan);
     ExitThread(0);
}

//////////////////////////////////////////////////////////////////////////////

/* continuously create threads to connect to a server */
static VOID WINAPI _scan(LPVOID lpBuffer)
{
     extern BOOL g_bAttack;
     HWND hwnd = (HWND)lpBuffer;
     HWND hHost = GetDlgItem(hwnd, IDC_PS_EDT_HOST);
     HWND hBtn  = GetDlgItem(hwnd, IDOK);
     HWND hList = GetDlgItem(hwnd, IDC_PS_LST);
     HWND hProg = GetDlgItem(hwnd, IDC_PS_PROGRESS);
     char* buf  = 0;
     int i;

     // get the hostname
     if(g_lpHostname) free(g_lpHostname);
     g_nPorts = 0;

     i = GetWindowTextLength(hHost);
     if(i <= 0)
     {
         err("Enter a host");
         ExitThread(0);
     }
     g_lpHostname = calloc(i + 1, sizeof(char));
     GetWindowText(hHost, g_lpHostname, i + 1);

     if(!g_bAttack)
        if(!start_winsock())
            ExitThread(0);

     if(!resolve_ip(g_lpHostname))
     {
         err("Invalid host");
         ExitThread(0);
     }

     // reset the listview and print status
     SendMessage(hList, LB_RESETCONTENT, 0, 0);
     buf = calloc(strlen(g_lpHostname) + 64, sizeof(char));
     wsprintf(buf, "Initiating TCP scan on %s", g_lpHostname);
     SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)buf);

     // reset the progress bar
     SendMessage(hProg, PBM_SETRANGE, 0, MAKELPARAM(0, g_nPortEnd - g_nPortStart));
     SendMessage(hProg, PBM_SETPOS, (WPARAM)0, 0);

     SetWindowText(hBtn, "Stop");
     g_bScanning = TRUE;
     g_nConnections = 0;

     for(i=g_nPortStart; i<=g_nPortEnd; ++i)
     {
         while(g_nConnections >= g_nMaxConnections)
            Sleep(100);

         struct scan_t* scan = malloc(sizeof(struct scan_t));

         scan->port = i;
         CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&_tcp_scan, scan, 0, 0);
         ++g_nConnections;

         SendMessage(hProg, PBM_DELTAPOS, (WPARAM)1, 0);

         if(!g_bScanning) break;
     }

     EnableWindow(hBtn, FALSE);

     // if user did not explicity stop the scan
     if(g_bScanning)
     {
         SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)"Threads initialized, waiting for possible timeouts...");

         Sleep(2000);

         g_bScanning = FALSE;
     }

     SetWindowText(hBtn, "Scan for listening ports");
     EnableWindow(hBtn, TRUE);
     SendMessage(hProg, PBM_SETPOS, (WPARAM)PORT_END, 0);
     wsprintf(buf, "Scan complete on %s, %d ports found open", g_lpHostname, g_nPorts);
     SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)buf);

     if(!g_bAttack) WSACleanup();
     free(buf);
}

//////////////////////////////////////////////////////////////////////////////

static LRESULT CALLBACK PsProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
     switch(msg)
     {
         case WM_CREATE:
            {
                HINSTANCE hInst = GetModuleHandle(NULL);
                HWND    hChild,
                        hTarget = GetDlgItem(g_hUnicorn, IDC_EDT_TARGET);
                HFONT   hFont;
                RECT    rc;
                HDC     hdc = GetDC(HWND_DESKTOP);
                char*   buf = 0;
                int     len;

                hFont = CreateFont(-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                                   0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
                                   CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT(UNICORN_FONT));

                GetClientRect(hwnd, &rc);

                // get the target
                len = GetWindowTextLength(hTarget);
                if(len > 0)
                {
                    buf = calloc(len + 1, sizeof(char));

                    GetWindowText(hTarget, buf, len + 1);
                }

                // host edit
                hChild = CreateWindowEx(WS_EX_CLIENTEDGE,
                                        WC_EDIT,
                                        (buf) ? buf : TEXT("127.0.0.1"),
                                        ES_AUTOHSCROLL |
                                            WS_VISIBLE | WS_TABSTOP | WS_CHILD,
                                        1, 1, rc.right / 2, 22,
                                        hwnd, (HMENU)IDC_PS_EDT_HOST, hInst, NULL);
                SendMessage(hChild, WM_SETFONT, (WPARAM)hFont, FALSE);

                hFont = CreateFont(-MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                                    0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
                                    CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT(UNICORN_FONT));

                // scan button
                hChild = CreateWindowEx(0,
                                        WC_BUTTON,
                                        TEXT("Scan for listening ports"),
                                        BS_PUSHBUTTON | BS_TEXT |
                                            WS_VISIBLE | WS_TABSTOP | WS_CHILD,
                                        rc.right / 2 + 1, 1, rc.right / 2, 22,
                                        hwnd, (HMENU)IDOK, hInst, NULL);
                SendMessage(hChild, WM_SETFONT, (WPARAM)hFont, FALSE);

                // progress bar
                hChild = CreateWindowEx(0,
                                        PROGRESS_CLASS,
                                        0,
                                        PBS_SMOOTH |
                                            WS_VISIBLE | WS_CHILD,
                                        0, rc.bottom - 16, rc.right, 16,
                                        hwnd, (HMENU)IDC_PS_PROGRESS, hInst, NULL);
                SendMessage(hChild, WM_SETFONT, (WPARAM)hFont, FALSE);
                SendMessage(hChild, PBM_SETRANGE, 0, MAKELPARAM(0, g_nPortEnd - g_nPortStart));

                hFont = CreateFont(-MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                        0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
                        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("Courier New"));

                // listbox
                hChild = CreateWindowEx(0,
                                        WC_LISTBOX,
                                        0,
                                        LBS_NOTIFY |
                                            WS_VISIBLE | WS_VSCROLL | WS_CHILD,
                                        0, 23, rc.right, rc.bottom - 40,
                                        hwnd, (HMENU)IDC_PS_LST, hInst, NULL);
                SendMessage(hChild, WM_SETFONT, (WPARAM)hFont, FALSE);

                if(buf) free(buf);
                ReleaseDC(HWND_DESKTOP, hdc);
            }
            return 0;

        case WM_GETMINMAXINFO:
            {
                LPMINMAXINFO lpInfo = (LPMINMAXINFO)lParam;

                if(lpInfo)
                {
                    lpInfo->ptMinTrackSize.x = 385;
                    lpInfo->ptMinTrackSize.y = 425;
                }
            }
            return 0;

        case WM_SIZE:
            {
                RECT rc = {0};

                GetClientRect(hwnd, &rc);

                SetWindowPos(GetDlgItem(hwnd, IDC_PS_EDT_HOST), NULL,
                             1, 1,
                             rc.right / 2, 22,
                             SWP_NOZORDER | SWP_NOMOVE);

                SetWindowPos(GetDlgItem(hwnd, IDOK), NULL,
                             rc.right / 2 + 1, 1, rc.right / 2, 22,
                             SWP_NOZORDER);

                SetWindowPos(GetDlgItem(hwnd, IDC_PS_LST), NULL,
                             0, 23,
                             rc.right, rc.bottom - 40,
                             SWP_NOZORDER | SWP_NOMOVE);

                SetWindowPos(GetDlgItem(hwnd, IDC_PS_PROGRESS), NULL,
                             0, rc.bottom - 16, rc.right, 16,
                             SWP_NOZORDER);
            }
            return 0;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDOK:
                    if(!g_bScanning)
                        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&_scan, hwnd, 0, 0);
                    else
                        g_bScanning = FALSE;
                    break;

                case IDC_PS_LST:
                    if(HIWORD(wParam) == LBN_DBLCLK)
                    {
                        HWND    hList = GetDlgItem(hwnd, IDC_PS_LST);
                        char*   buf;
                        int     index,
                                len,
                                port;

                        index = SendMessage(hList, LB_GETCURSEL, 0, 0);
                        len   = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)index, 0);

                        if(len > 0)
                        {
                            buf = calloc(len + 1, sizeof(char));
                            if(SendMessage(hList, LB_GETTEXT, (WPARAM)index, (LPARAM)buf) == LB_ERR)
                            {
                                free(buf);
                                err("Error retrieving selected text");
                                break;
                            }

                            if(strncmp(buf, "Found", 5) != 0)
                            {
                                free(buf);
                                err("Double click an open port to set it as the target port");
                                break;
                            }

                            sscanf(buf, "Found open port %d on *", &port);
                            wsprintf(buf, "%d", port);
                            SetDlgItemText(g_hUnicorn, IDC_EDT_PORT, buf);
                            free(buf);
                        }
                    }
                    break;

                default:
                    break;
            }
            return 0;

        case WM_DESTROY:
            if(g_lpHostname) free(g_lpHostname);
            DestroyWindow(hwnd);
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
     }

     return 1;
}

//////////////////////////////////////////////////////////////////////////////

/* port scanner options procedure */
BOOL CALLBACK PsOpProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HWND hStart = GetDlgItem(hwnd, IDC_PS_OP_EDT1);
    HWND hEnd   = GetDlgItem(hwnd, IDC_PS_OP_EDT2);

    switch(msg)
    {
        case WM_INITDIALOG:
            {
                char buf[256] = {0};

                wsprintf(buf, "%d", g_nPortStart);
                SetWindowText(hStart, buf);

                wsprintf(buf, "%d", g_nPortEnd);
                SetWindowText(hEnd, buf);
            }
            return 0;

        case WM_COMMAND:
            if(LOWORD(wParam) == IDOK)
            {
                char* buf = 0;
                int   len;

                len = GetWindowTextLength(hStart);
                if(len > 0)
                {
                    buf = calloc(len + 1, sizeof(char));
                    GetWindowText(hStart, buf, len + 1);
                    sscanf(buf, "%d", &g_nPortStart);
                    free(buf);
                }

                len = GetWindowTextLength(hEnd);
                if(len > 0)
                {
                    buf = calloc(len + 1, sizeof(char));
                    GetWindowText(hEnd, buf, len + 1);
                    sscanf(buf, "%d", &g_nPortEnd);
                    free(buf);
                }

                if(g_nPortStart > g_nPortEnd
                   || g_nPortStart < 0
                   || g_nPortEnd > PORT_END)
                {
                    err("Invalid port range");
                    g_nPortStart    = 1;
                    g_nPortEnd      = 1024;
                    return 0;
                }

                EndDialog(hwnd, IDOK);
            }
            if(LOWORD(wParam) == IDCANCEL)
                EndDialog(hwnd, IDCANCEL);
            return 0;

        case WM_CLOSE:
            EndDialog(hwnd, 0);
            return 0;

        default:
            return 0;
    }

    return 1;
}

//////////////////////////////////////////////////////////////////////////////

/* register a new window class */
void register_ps_class(void)
{
    WNDCLASSEX  wincl;
    HINSTANCE   hInst = GetModuleHandle(NULL);

    memset(&wincl, 0, sizeof(WNDCLASSEX));

    wincl.hInstance     = hInst;
    wincl.lpszClassName = WC_PS;
    wincl.lpfnWndProc   = PsProc;
    wincl.style         = CS_HREDRAW | CS_VREDRAW;
    wincl.cbSize        = sizeof(WNDCLASSEX);
    wincl.hIcon         = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MAIN));
    wincl.hIconSm       = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MAIN));
    wincl.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName  = NULL;
    wincl.cbClsExtra    = 0;
    wincl.cbWndExtra    = 0;
    wincl.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

    if(!RegisterClassEx(&wincl))
        err("Unable to register class for active connections window");
}

//////////////////////////////////////////////////////////////////////////////

/* catch hot keys before they are processed */
static int process_hotkeys(const HWND hwnd, const MSG* msg)
{
    if(msg->message == WM_KEYDOWN)
    {
        if(msg->wParam == VK_RETURN)
        {
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), 0);
            return 1;
        }
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////

/* create a new port scanner window */
VOID WINAPI _ps(void)
{
    HINSTANCE   hInst = GetModuleHandle(NULL);
    HWND        hwnd;
    MSG         msg;

    /* Use this scope for variables not needed later */
    {
        DWORD dwStyle;
        DWORD dwExStyle;

        /* Configure window styles */
        dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
        dwExStyle = WS_EX_APPWINDOW;

        /* Get the windows version and apply necessary styles */
        if(get_os_ver() >= VER_XP)
            dwExStyle |= WS_EX_COMPOSITED;

        /* Create the main window */
        hwnd = CreateWindowEx(0,
                              WC_PS,
                              TEXT("Port Scanner - UDP Unicorn"),
                              dwStyle,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              385,
                              425,
                              HWND_DESKTOP,
                              NULL,
                              hInst,
                              NULL
                              );
    }

    if(!hwnd)
    {
        MessageBox(NULL, "Failed to create window for port scanner", 0, MB_ICONERROR);
        ExitThread(0);
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    while(GetMessage(&msg, NULL, 0, 0))
    {
        if(!process_hotkeys(hwnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    ExitThread(0);
}
