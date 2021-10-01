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

#include "ac.h"
#include "ps.h"

HWND    g_hUnicorn;
UINT    g_filter_port = 0;

typedef DWORD (WINAPI *pGetExtendedTcpTable)(PVOID pTcpTable, PDWORD pdwSize, BOOL bOrder, ULONG ulAf, TCP_TABLE_CLASS TableClass, ULONG Reserved);
typedef DWORD (WINAPI *pGetExtendedUdpTable)(PVOID pUdpTable, PDWORD pdwSize, BOOL bOrder, ULONG ulAf, UDP_TABLE_CLASS TableClass, ULONG Reserved);

//////////////////////////////////////////////////////////////////////////////

/* add info to the listview from a structure */
static void update_table_listview(const struct table_t* data)
{
    HWND    hList       = GetDlgItem(FindWindow(WC_AC, NULL), IDC_AC_LST);
    LVITEM  li          = {0};
    char    buf[1024]   = {0};
    int     index;

    // set up LVITEM struct
    li.mask         = LVIF_TEXT | LVIF_PARAM;
    li.iSubItem     = 0;
    li.pszText      = buf;
    li.cchTextMax   = sizeof(buf);
    li.lParam       = (DWORD)data->pid;

    // insert the protocol
    strcpy(buf, (data->proto == PROTO_TCP) ? "TCP" : "UDP");
    index = SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&li);

    // set the source address and port
    li.iSubItem = 1;
    wsprintf(buf, "%s:%d", data->local, data->loc_port);
    SendMessage(hList, LVM_SETITEMTEXT, (WPARAM)index, (LPARAM)&li);

    // set the text for the remote address and port
    li.iSubItem = 2;
    if(data->proto == PROTO_TCP)
        wsprintf(buf, "%s:%d", data->remote, data->rem_port);
    else
        strcpy(buf, "*:*");

    SendMessage(hList, LVM_SETITEMTEXT, (WPARAM)index, (LPARAM)&li);

    if(data->proto == PROTO_TCP)
    {
        // set state
        li.iSubItem = 3;
        switch(data->state)
        {
            case MIB_TCP_STATE_CLOSED:
                strcpy(buf, "CLOSED");
                break;

            case MIB_TCP_STATE_LISTEN:
                strcpy(buf, "LISTENING");
                break;

            case MIB_TCP_STATE_SYN_SENT:
                strcpy(buf, "SYN-SENT");
                break;

            case MIB_TCP_STATE_SYN_RCVD:
                strcpy(buf, "SYN-RECEIVED");
                break;

            case MIB_TCP_STATE_ESTAB:
                strcpy(buf, "ESTABLISHED");
                break;

            case MIB_TCP_STATE_FIN_WAIT1:
                strcpy(buf, "FIN-WAIT-1");
                break;

            case MIB_TCP_STATE_FIN_WAIT2:
                strcpy(buf, "FIN-WAIT-2");
                break;

            case MIB_TCP_STATE_CLOSE_WAIT:
                strcpy(buf, "CLOSE-WAIT");
                break;

            case MIB_TCP_STATE_CLOSING:
                strcpy(buf, "CLOSING");
                break;

            case MIB_TCP_STATE_LAST_ACK:
                strcpy(buf, "LAST-ACK");
                break;

            case MIB_TCP_STATE_TIME_WAIT:
                strcpy(buf, "TIME-WAIT");
                break;

            case MIB_TCP_STATE_DELETE_TCB:
                strcpy(buf, "DELETE-TCB");
                break;

            default:
                strcpy(buf, "UNKNOWN");
                break;
        }

        SendMessage(hList, LVM_SETITEMTEXT, (WPARAM)index, (LPARAM)&li);
    }
}

//////////////////////////////////////////////////////////////////////////////

/* acquire the TCP table in Windows */
static void get_tcp_table(void)
{
    PMIB_TCPTABLE_OWNER_PID table;
    struct table_t  data;
    struct in_addr  addr;
    DWORD           size,
                    i;

    pGetExtendedTcpTable _GetExtendedTcpTable;

    memset(&data, 0, sizeof(struct table_t));

    // load function pointer
    _GetExtendedTcpTable = (pGetExtendedTcpTable)GetProcAddress(LoadLibrary("iphlpapi.dll"), "GetExtendedTcpTable");
    if(_GetExtendedTcpTable == NULL)
    {
        err("Unable to load TCP table. Failed to retrieve function pointer to \"GetExtendedTcpTable()\"");
        return;
    }

    size = sizeof(PMIB_TCPTABLE_OWNER_PID);

    table = (PMIB_TCPTABLE_OWNER_PID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);

    if(_GetExtendedTcpTable(table, &size, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == ERROR_INSUFFICIENT_BUFFER)
    {
        HeapFree(GetProcessHeap(), 0, table);

        // allocate correct amount of memory
        table = (PMIB_TCPTABLE_OWNER_PID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
        if(!table)
        {
            err("Unable to allocate enough memory for the TCP table");
            return;
        }
    }

    if(_GetExtendedTcpTable(table, &size, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == NO_ERROR)
    {
        for(i=0; i<table->dwNumEntries; ++i)
        {
            // store the local and remote IP addresses
            addr.S_un.S_addr = (u_long)table->table[i].dwLocalAddr;
            strcpy(data.local, inet_ntoa(addr));

            addr.S_un.S_addr = (u_long)table->table[i].dwRemoteAddr;
            strcpy(data.remote, inet_ntoa(addr));

            // store local and remote port
            data.loc_port = ntohs((u_short)table->table[i].dwLocalPort);
            data.rem_port = ntohs((u_short)table->table[i].dwRemotePort);

            // store the state
            data.state = table->table[i].dwState;

            // store the PID
            data.pid = table->table[i].dwOwningPid;

            // set protocol
            data.proto = PROTO_TCP;

            if(g_filter_port > 0)
            {
                if(g_filter_port == data.rem_port)
                    update_table_listview(&data);
            }
            else
                update_table_listview(&data);
        }
    }

    if(table)
        HeapFree(GetProcessHeap(), 0, table);
}

//////////////////////////////////////////////////////////////////////////////

/* procedure for port filter dialog */
static BOOL CALLBACK FilterProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HWND hPort = GetDlgItem(hwnd, IDC_FILTER_EDT_PORT);

    switch(msg)
    {
        case WM_INITDIALOG:
            {
                if(g_filter_port > 0)
                {
                    char buf[32] = {0};

                    wsprintf(buf, "%u", g_filter_port);

                    SetWindowText(hPort, buf);
                }
                else
                    SetWindowText(hPort, "0");

                SetFocus(hPort);
                SendMessage(hPort, EM_SETSEL, (WPARAM)0, (LPARAM)GetWindowTextLength(hPort));
            }
            return 0;

        case WM_COMMAND:
            if(LOWORD(wParam) == IDOK)
            {
                char    buf[32] = {0};
                int     len;

                len = GetWindowTextLength(hPort);
                if(len > 0)
                {
                    if(len > 31)
                    {
                        err("Port is too large");
                        break;
                    }

                    GetWindowText(hPort, buf, len + 1);
                    sscanf(buf, "%u", &g_filter_port);
                    EndDialog(hwnd, IDOK);
                }
                else
                    err("Enter a port to filter");
            }
            else if(LOWORD(wParam) == IDCANCEL)
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

/* callback procedure for active connections window */
static LRESULT CALLBACK AcProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
            {
                HWND    hList;
                HFONT   hFont;
                RECT    rc;
                HDC     hdc = GetDC(HWND_DESKTOP);
                LVCOLUMN lc = {0};

                lc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
                lc.fmt  = LVCFMT_LEFT;

                hFont = CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                                   0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
                                   CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("Courier New"));

                ReleaseDC(HWND_DESKTOP, hdc);

                GetClientRect(hwnd, &rc);

                // create listview
                hList = CreateWindowEx(WS_EX_CLIENTEDGE,
                                       WC_LISTVIEW,
                                       0,
                                       LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL |
                                            WS_VISIBLE | WS_CHILD,
                                       0, 0, rc.right, rc.bottom,
                                       hwnd, (HMENU)IDC_AC_LST, GetModuleHandle(NULL), NULL);
                SendMessage(hList, WM_SETFONT, (WPARAM)hFont, FALSE);
                ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

                // add columns
                lc.iSubItem = 0;
                lc.pszText  = "State";
                lc.cx       = 103;
                SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&lc);

                lc.iSubItem = 1;
                lc.pszText  = "Remote Address";
                lc.cx       = 155;
                SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&lc);

                lc.iSubItem = 2;
                lc.pszText  = "Local Address";
                lc.cx       = 155;
                SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&lc);

                lc.iSubItem = 3;
                lc.pszText  = "Proto";
                lc.cx       = 50;
                SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&lc);

                get_udp_table();
            }
            return 0;

        case WM_SIZE:
            {
                RECT rc;

                GetClientRect(hwnd, &rc);

                SetWindowPos(GetDlgItem(hwnd, IDC_AC_LST), NULL,
                             0,
                             0,
                             rc.right,
                             rc.bottom,
                             SWP_NOZORDER | SWP_NOMOVE);
            }
            return 0;

        case WM_NOTIFY:
            switch(((LPNMHDR)lParam)->code)
            {
                case NM_RCLICK:
                    // show context menu
                    if(((LPNMHDR)lParam)->idFrom == IDC_AC_LST)
                    {
                        POINT p;
                        HMENU hContext = CreatePopupMenu();

                        GetCursorPos(&p);

                        AppendMenu(hContext, MF_STRING, IDR_AC_REFRESH, "Refresh\tF5");
                        AppendMenu(hContext, MF_SEPARATOR, 0, 0);
                        AppendMenu(hContext, MF_STRING, IDR_AC_SET_TARGET, "Set Remote Address as Target");
                        AppendMenu(hContext, MF_STRING, IDR_AC_SENDTO_PS, "Send Remote Address to Port Scanner");
                        AppendMenu(hContext, MF_SEPARATOR, 0, 0);
                        AppendMenu(hContext, MF_STRING, IDR_AC_FILTER_PORT, "Filter Port");

                        TrackPopupMenuEx(hContext,
                                         TPM_LEFTALIGN | TPM_LEFTBUTTON,
                                         p.x,
                                         p.y,
                                         hwnd,
                                         NULL);
                    }
                    break;

                case NM_CUSTOMDRAW:
                    // color every other listview item gray
                    if(((LPNMHDR)lParam)->idFrom == IDC_AC_LST)
                    {
                        LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;

                        switch(lplvcd->nmcd.dwDrawStage)
                        {
                            case CDDS_PREPAINT:

                                return CDRF_NOTIFYITEMDRAW;

                            case CDDS_ITEMPREPAINT:
                            case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
                                if(lplvcd->nmcd.dwItemSpec % 2 != 0)
                                    lplvcd->clrTextBk = RGB(240, 238, 238);

                                return CDRF_NEWFONT;
                        }

                        return CDRF_NEWFONT;
                    }
                    break;

                default:
                    break;
            }
            return 0;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDR_AC_REFRESH:
                    get_udp_table();
                    break;

                case IDR_AC_SET_TARGET:
                    {
                        HWND    hList       = GetDlgItem(hwnd, IDC_AC_LST);
                        HWND    hTarget     = GetDlgItem(g_hUnicorn, IDC_EDT_TARGET);
                        int     index       = ListView_GetNextItem(hList, -1, LVNI_SELECTED),
                                i;
                        char    buf[512]    = {0};

                        ListView_GetItemText(hList, index, 2, buf, sizeof(buf));

                        // remove the port number and colon
                        for(i=strlen(buf); buf[i] != ':'; --i)
                            buf[i] = '\0';

                        buf[i] = '\0';

                        SetWindowText(hTarget, buf);
                        SetForegroundWindow(g_hUnicorn);
                        SetFocus(hTarget);
                        SendMessage(hTarget, EM_SETSEL, (WPARAM)0, (LPARAM)GetWindowTextLength(hTarget));
                    }
                    break;

                case IDR_AC_SENDTO_PS:
                    {
                        HWND hList      = GetDlgItem(hwnd, IDC_AC_LST);
                        HWND hPs        = FindWindow(WC_PS, NULL);
                        char buf[256]   = {0};
                        int  i;

                        if(hPs == NULL)
                        {
                            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&_ps, NULL, 0, 0);
                            Sleep(50);
                            hPs = FindWindow(WC_PS, NULL);
                        }
                        else
                            SetForegroundWindow(hPs);

                        ListView_GetItemText(hList, ListView_GetNextItem(hList, -1, LVNI_SELECTED), 2, buf, sizeof(buf));

                        // remove port number and colon
                        for(i=strlen(buf); buf[i] != ':'; --i)
                            buf[i] = '\0';

                        buf[i] = '\0';

                        SetWindowText(GetDlgItem(hPs, IDC_PS_EDT_HOST), buf);
                    }
                    break;

                case IDR_AC_FILTER_PORT:
                    if(DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(DLG_FILTER), hwnd, FilterProc) == IDOK)
                        SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDR_AC_REFRESH, BN_CLICKED), 0);
                    break;

                default:
                    break;
            }
            return 0;

        case WM_DESTROY:
            DestroyWindow(hwnd);
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 1;
}

//////////////////////////////////////////////////////////////////////////////

/* catch hot keys before they are processed */
static int process_hotkeys(const HWND hwnd, const MSG* msg)
{
    if(msg->message == WM_KEYDOWN)
    {
        if(msg->wParam == VK_F5)
        {
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDR_AC_REFRESH, BN_CLICKED), 0);
            return 1;
        }
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////

/* register a new window class */
void register_ac_class(void)
{
    WNDCLASSEX  wincl;
    HINSTANCE   hInst = GetModuleHandle(NULL);

    memset(&wincl, 0, sizeof(WNDCLASSEX));

    wincl.hInstance     = hInst;
    wincl.lpszClassName = WC_AC;
    wincl.lpfnWndProc   = AcProc;
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

/* create a new active connections window */
VOID WINAPI _ac(void)
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
                              WC_AC,
                              TEXT("Active Connections - UDP Unicorn"),
                              dwStyle,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              500,
                              475,
                              HWND_DESKTOP,
                              NULL,
                              hInst,
                              NULL
                              );
    }

    if(!hwnd)
    {
        MessageBox(NULL, "Failed to create window for active connections", 0, MB_ICONERROR);
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

//////////////////////////////////////////////////////////////////////////////

/* get the UDP table from Windows */
void get_udp_table(void)
{
    PMIB_UDPTABLE_OWNER_PID table;
    struct table_t  data;
    struct in_addr  addr;
    DWORD           size,
                    i;

    pGetExtendedUdpTable _GetExtendedUdpTable;

    memset(&data, 0, sizeof(struct table_t));

    // load function pointer
    _GetExtendedUdpTable = (pGetExtendedUdpTable)GetProcAddress(LoadLibrary("iphlpapi.dll"), "GetExtendedUdpTable");
    if(_GetExtendedUdpTable == NULL)
    {
        err("Unable to load UDP table. Failed to retrieve function pointer to \"GetExtendedUdpTable()\"");
        get_tcp_table();
        return;
    }

    table = (PMIB_UDPTABLE_OWNER_PID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MIB_UDPTABLE_OWNER_PID));

    size = sizeof(MIB_UDP6ROW_OWNER_PID);

    if(_GetExtendedUdpTable(table, &size, TRUE, AF_INET, UDP_TABLE_OWNER_PID, 0) == ERROR_INSUFFICIENT_BUFFER)
    {
        HeapFree(GetProcessHeap(), 0, table);

        // allocate correct amount of memory
        table = (PMIB_UDPTABLE_OWNER_PID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
        if(!table)
        {
            err("Unable to allocate enough memory for the UDP table");
            get_tcp_table();
            return;
        }
    }

    // clear listview
    SendMessage(GetDlgItem(FindWindow(WC_AC, NULL), IDC_AC_LST), LVM_DELETEALLITEMS, 0, 0);

    if(_GetExtendedUdpTable(table, &size, TRUE, AF_INET, UDP_TABLE_OWNER_PID, 0) == NO_ERROR)
    {
        for(i=0; i<table->dwNumEntries; ++i)
        {
            // store local address
            addr.S_un.S_addr = (u_long)table->table[i].dwLocalAddr;
            strcpy(data.local, inet_ntoa(addr));

            // store local port
            data.loc_port = ntohs((u_short)table->table[i].dwLocalPort);

            // store the pid
            data.pid = table->table[i].dwOwningPid;

            // set the protocol
            data.proto = PROTO_UDP;

            if(g_filter_port > 0)
            {
                if(g_filter_port == data.loc_port)
                    update_table_listview(&data);
            }
            else
                update_table_listview(&data);
        }
    }

    if(table)
        HeapFree(GetProcessHeap(), 0, table);

    get_tcp_table();
}

