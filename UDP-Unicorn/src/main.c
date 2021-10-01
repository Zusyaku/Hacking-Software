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

#include "udpunicorn.h"
#include "ac.h"
#include "ps.h"
#include "netinfo.h"
#include "config.h"
#include "music.h"
#include "resource.h"

#define WM_TRAYICON (WM_USER + 1)

//////////////////////////////////////////////////////////////////////////////

BOOL g_bAttack;
HWND g_hDataSent;
HBRUSH g_hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
unsigned long long g_nDataSent;

//////////////////////////////////////////////////////////////////////////////

/* add protocol/application choices to port box */
static void add_ports(HWND hPort)
{
    const char* ports[] = {"rand", _SSH, _TELNET, _SMTP, _DNS, _DHCP, _HTTP, _KERBEROS, _NETBIOS, _IMAP4,
                            _SNMP, _LDAP, _HTTPS, _RIP, _SFTP_SSL, _MS_SQL, _MSN, _MYSQL, _ITUNES, _VENT,
                            _ICQ, _AOL, _XFIRE, _XBOX_LIVE, _PSN, _COD, _MW2, _CSTRIKE, _MC, _TF2, _WOW,
                            _LOL, 0};
    int i;

    for(i=0; i<23; ++i)
        SendMessage(hPort, CB_ADDSTRING, 0, (LPARAM)ports[i]);

    SendMessage(hPort, CB_ADDSTRING, 0, (LPARAM)"-------");

    for(; i<32; ++i)
        SendMessage(hPort, CB_ADDSTRING, 0, (LPARAM)ports[i]);
}

//////////////////////////////////////////////////////////////////////////////

/* procedure for main window */
static BOOL CALLBACK DlgHelpProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_NOTIFY:
            switch(((LPNMHDR)lParam)->code)
            {
                case NM_CLICK:
                case NM_RETURN:
                    {
                        if(((LPNMHDR)lParam)->idFrom == IDC_ABOUT_LINK)
                        {
                            if((int)ShellExecute(NULL, "open", "http://udpunicorn.sourceforge.net/", 0, 0, SW_SHOWNORMAL) <= 32)
                                MessageBox(hwnd, "Failed to open URL: \"http://udpunicorn.sourceforge.net/\"", "Error", MB_OK | MB_ICONERROR);
                        }
                    }
                    break;

                default:
                    break;
            }
            return 0;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    break;

                default:
                    break;
            }
            return 0;

        default:
            return FALSE;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

static BOOL CALLBACK DlgMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HWND hEditTarget        = GetDlgItem(hwnd, IDC_EDT_TARGET),
         hEditPort          = GetDlgItem(hwnd, IDC_EDT_PORT),
         hTbSize            = GetDlgItem(hwnd, IDC_TB_SIZE),
         hEditSize          = GetDlgItem(hwnd, IDC_EDT_SIZE),
         hEditDelay         = GetDlgItem(hwnd, IDC_EDT_DELAY),
         hEditThreads       = GetDlgItem(hwnd, IDC_EDT_THREADS),
         hEditSockets       = GetDlgItem(hwnd, IDC_EDT_SOCKETS),
         hBtnTest           = GetDlgItem(hwnd, IDC_BTN_TEST),
         hEditTest          = GetDlgItem(hwnd, IDC_EDT_TEST),
         hBtnDNS            = GetDlgItem(hwnd, IDC_BTN_DNS),
         hEditDNS           = GetDlgItem(hwnd, IDC_EDT_DNS),
         hBtnAttack         = GetDlgItem(hwnd, IDC_BTN_ATTACK),
         hEditDataSent      = GetDlgItem(hwnd, IDC_EDT_DATASENT),
         hBtnResetData      = GetDlgItem(hwnd, IDC_BTN_RESETDATA),
         hBtnMusic          = GetDlgItem(hwnd, IDC_BTN_MUSIC),
         hIcoLeftI          = GetDlgItem(hwnd, IDC_ICO_LEFTI),
         hIcoRightI         = GetDlgItem(hwnd, IDC_ICO_RIGHTI),
         hIcoLeftA          = GetDlgItem(hwnd, IDC_ICO_LEFTA),
         hIcoRightA         = GetDlgItem(hwnd, IDC_ICO_RIGHTA);

    switch(msg)
    {
        case WM_INITDIALOG:
            {
                HINSTANCE hInst = GetModuleHandle(NULL);
                INITCOMMONCONTROLSEX iccx;

                // set default values for each edit box
                SetWindowText(hEditTarget, "127.0.0.1");
                SetWindowText(hEditPort, "rand");
                SetWindowText(hEditSize, "10.0");
                SetWindowText(hEditDelay, "10");
                SetWindowText(hEditThreads, "1");
                SetWindowText(hEditSockets, "1");
                SetWindowText(hEditTest, "Idle");
                SetWindowText(hEditDNS, "Idle");
                SetWindowText(hEditDataSent, "0");

                SendMessage(hTbSize, TBM_SETRANGE, TRUE, MAKELONG(1, 65));
                SendMessage(hTbSize, TBM_SETPOS, TRUE, 10);

                // load visual theme and controls
                iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
                iccx.dwICC = ICC_BAR_CLASSES | ICC_LINK_CLASS | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS
                            | ICC_STANDARD_CLASSES | ICC_USEREX_CLASSES;
                InitCommonControlsEx(&iccx);

                // load menu
                HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU));
                SetMenu(hwnd, hMenu);

                // load main icon
                HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MAIN));
                SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM) hIcon);

                // configure icons
                ShowWindow(hIcoLeftA, FALSE);
                ShowWindow(hIcoRightA, FALSE);

                // seed random number generator
                srand(GetTickCount());

                // add ports to combobox
                add_ports(hEditPort);

                // set music button icon
                HICON hMusic = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_MUSIC),
                                                IMAGE_ICON, 32, 32, LR_SHARED);
                SendMessage(hBtnMusic, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hMusic);

                // register window classes
                register_ac_class();
                register_ps_class();

                // set global window handle from ac.c
                g_hUnicorn = hwnd;
                g_hDataSent = GetDlgItem(hwnd, IDC_EDT_DATASENT);

                // read configuration options
                read_config();
            }
            return 0;

        case WM_CTLCOLORDLG:
            return (LONG)g_hbrBackground;

        case WM_CTLCOLORSTATIC:
            {
                HDC     hdc = (HDC)wParam;
                HWND    hCtl = (HWND)lParam;

                SetBkMode(hdc, TRANSPARENT);

                if(hCtl == GetDlgItem(hwnd, IDC_CHK_RANDSIZE))
                    return (LRESULT)GetStockObject(NULL_BRUSH);
            }
            return (LONG)g_hbrBackground;

        case WM_COMMAND:
            {
                switch(LOWORD(wParam))
                {
                    // menu
                    case IDR_FILE_RESET:
                        SetWindowText(hEditTarget, "127.0.0.1");
                        SetWindowText(hEditPort, "rand");
                        SetWindowText(hEditSize, "10.0");
                        SetWindowText(hEditDelay, "10");
                        SetWindowText(hEditThreads, "1");
                        SetWindowText(hEditSockets, "1");
                        SetWindowText(hEditTest, "Idle");
                        SetWindowText(hEditDNS, "Idle");
                        SetWindowText(hEditDataSent, "0");

                        SendMessage(hTbSize, TBM_SETRANGE, TRUE, MAKELONG(1, 65));
                        SendMessage(hTbSize, TBM_SETPOS, TRUE, 10);

                        EnableWindow(hTbSize, TRUE);
                        EnableWindow(hEditSize, TRUE);
                        SendMessage(GetDlgItem(hwnd, IDC_CHK_RANDSIZE), BM_SETCHECK, (WPARAM) FALSE, 0);

                        g_nDataSent = 0;
                        break;

                    case IDR_FILE_QUIT:
                        save_config();
                        EndDialog(hwnd, 0);
                        break;

                    case IDR_TOOLS_CONNECT:
                        if(FindWindow(WC_AC, NULL) == NULL)
                            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&_ac, NULL, 0, 0);
                        else
                            SetForegroundWindow(FindWindow(WC_AC, NULL));
                        break;

                    case IDR_TOOLS_PORTSCAN:
                        if(FindWindow(WC_PS, NULL) == NULL)
                            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&_ps, NULL, 0, 0);
                        else
                            SetForegroundWindow(FindWindow(WC_PS, NULL));
                        break;

                    case IDR_TOOLS_NETINFO:
                        if(FindWindow(NULL, "Network Info") == NULL)
                            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(DLG_NETINFO), NULL, NetInfoProc);
                        else
                            SetForegroundWindow(FindWindow(NULL, "Network Info"));
                        break;

                    case IDR_TOOLS_PORTLIST:
                        if((int)ShellExecute(NULL, "open", "http://portforward.com/cports.htm", 0, 0, SW_SHOWNORMAL) <= 32)
                            MessageBox(hwnd, "Failed to open URL: \"http://portforward.com/cports.htm\"", 0, MB_ICONERROR);
                        break;

                    case IDR_OP_TOP:
                        {
                            MENUITEMINFO    item  = {0};
                            HMENU           hMenu = GetMenu(hwnd);

                            item.cbSize     = sizeof(MENUITEMINFO);
                            item.fMask      = MIIM_STATE;

                            if(GetMenuItemInfo(hMenu, IDR_OP_TOP, FALSE, &item))
                            {
                                if(item.fState == MFS_CHECKED)
                                {
                                    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                                    CheckMenuItem(hMenu, IDR_OP_TOP, MF_UNCHECKED);
                                }
                                else
                                {
                                    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                                    CheckMenuItem(hMenu, IDR_OP_TOP, MF_CHECKED);
                                }
                            }
                        }
                        break;

                    case IDR_OP_MIN_TRAY:
                        {
                            MENUITEMINFO    item = {0};
                            HMENU           hMenu = GetMenu(hwnd);

                            item.cbSize     = sizeof(MENUITEMINFO);
                            item.fMask      = MIIM_STATE;

                            if(GetMenuItemInfo(hMenu, IDR_OP_MIN_TRAY, FALSE, &item))
                            {
                                if(item.fState == MFS_CHECKED)
                                {
                                    SetWindowLongPtr(hwnd, GWL_USERDATA, MIN_SHOW);
                                    CheckMenuItem(hMenu, IDR_OP_MIN_TRAY, MF_UNCHECKED);
                                }
                                else
                                {
                                    SetWindowLongPtr(hwnd, GWL_USERDATA, MIN_HIDE);
                                    CheckMenuItem(hMenu, IDR_OP_MIN_TRAY, MF_CHECKED);
                                }
                            }
                        }
                        break;

                    case IDR_OP_BGCOLOR:
                        {
                            // create a color choosing dialog thing
                            static COLORREF crCustom[16] = {0};

                            COLORREF crNewBgr = RGB(255, 255, 255);
                            CHOOSECOLOR cc = {sizeof(CHOOSECOLOR)};
                            RECT rc = {0};

                            cc.hwndOwner = hwnd;
                            cc.rgbResult = crNewBgr;
                            cc.lpCustColors = crCustom;
                            cc.Flags = CC_RGBINIT | CC_FULLOPEN | CC_ANYCOLOR;

                            if(ChooseColor(&cc))
                            {
                                crNewBgr = cc.rgbResult;
                                g_hbrBackground = CreateSolidBrush(cc.rgbResult);

                                // set the background handle to the window class
                                //  and delete the brush
                                DeleteObject((HBRUSH)SetClassLong(hwnd, GCL_HBRBACKGROUND,
                                                                  (LONG)CreateSolidBrush(crNewBgr)));

                                // update window background
                                GetClientRect(hwnd, &rc);
                                InvalidateRect(hwnd, &rc, TRUE);

                                ShowWindow(hTbSize, FALSE);
                                ShowWindow(hTbSize, TRUE);

                                UpdateWindow(hwnd);
                            }
                        }
                        break;

                    case IDR_OP_RESETBG:
                        {
                            RECT rc;

                            g_hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);

                            GetClientRect(hwnd, &rc);
                            InvalidateRect(hwnd, &rc, TRUE);

                            ShowWindow(hTbSize, FALSE);
                            ShowWindow(hTbSize, TRUE);

                            UpdateWindow(hwnd);
                        }
                        break;

                    case IDR_OP_PORTSCAN:
                        DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(DLG_PS_OP), hwnd, PsOpProc);
                        break;

                    case IDR_OP_CONFIG:
                        if((int)ShellExecute(NULL, "edit", CONFIG_FILE, 0, 0, SW_SHOWNORMAL) <= 32)
                            err("Failed to open \"data\\config.ini\" in edit mode");
                        break;

                    case IDR_HELP_FAQ:
                        MessageBox(NULL, "Q: What's the deal with the port?\n"
                                   "A: You can type \"rand\" for flooding random ports or the port number to flood.\n\n"
                                   "Q: When I flood a target, my own connection fails, what's going on?\n"
                                   "A: Try blocking the target's replies with firewall software(COMODO or ZoneAlarm)"
                                   ". It can also be due to you having less bandwidth. For a DoS attack to work, "
                                   "you need a better connection than the target. "
                                   "Also, try attacking with different methods such as smaller packet size and more "
                                   "threads. Mess around with the delay as well.\n\n"
                                   "Q: What is the port filter in Active Connections?\n"
                                   "A: This only allows connections with the specified port to be displayed, set to 0 to disable.\n\n"
                                   "Q: What is this music the program that plays when I press the music button?\n"
                                   "A: It's a snippet of the song from Robot Unicorn Attack, a game which inspired me to create this program. If you haven't"
                                   " played the game, go check it out, it'll change your life. Also, you can change the music that plays"
                                   " by putting your own music file in the \"data\" folder and naming it \"music.mp3\" (file type doesn't matter).\n\n"
                                   "Note: Winsock in XP SP2 or later is not able to use raw sockets to spoof the source IP address, "
                                   "so ICMP replies will be directed at you, if you wish to spoof packets, use Linux.\n\n"
                                   "Make your dreams come true.",
                                   "FAQ",
                                   MB_ICONINFORMATION);
                        break;

                    case IDR_HELP_ABOUT:
                        DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(DLG_ABOUT), hwnd, DlgHelpProc);
                        break;

                    case IDR_TRAY_SHOW:
                        {
                            NOTIFYICONDATA  nid;
                            HWND            hChild;

                            nid.cbSize  = sizeof(NOTIFYICONDATA);
                            nid.hWnd    = hwnd;
                            nid.uID     = IDR_TRAYICON;
                            Shell_NotifyIcon(NIM_DELETE, &nid);

                            if((hChild = FindWindow(WC_AC, NULL)) != NULL)
                                ShowWindow(hChild, SW_SHOWNORMAL);
                            if((hChild = FindWindow(WC_PS, NULL)) != NULL)
                                ShowWindow(hChild, SW_SHOWNORMAL);
                            if((hChild = FindWindow(NULL, "Network Info")) != NULL)
                                ShowWindow(hChild, SW_SHOWNORMAL);
                            if((hChild = FindWindow(NULL, "Filter Port")) != NULL)
                                ShowWindow(hChild, SW_SHOWNORMAL);

                            ShowWindow(hwnd, SW_SHOWNORMAL);
                            SetForegroundWindow(hwnd);
                        }
                        break;

                    case IDR_TRAY_CLOSE:
                        {
                            NOTIFYICONDATA nid;

                            nid.cbSize  = sizeof(NOTIFYICONDATA);
                            nid.hWnd    = hwnd;
                            nid.uID     = IDR_TRAYICON;
                            Shell_NotifyIcon(NIM_DELETE, &nid);

                            save_config();
                            EndDialog(hwnd, 0);
                        }
                        break;

                    case IDC_CHK_RANDSIZE:
                        if( IsDlgButtonChecked( hwnd, IDC_CHK_RANDSIZE ) )
                        {
                            // disable packet size editing
                            EnableWindow(GetDlgItem(hwnd, IDC_TB_SIZE), FALSE);
                            EnableWindow(hEditSize, FALSE);
                        }
                        else
                        {
                            EnableWindow(GetDlgItem(hwnd, IDC_TB_SIZE), TRUE);
                            EnableWindow(hEditSize, TRUE);
                        }
                        break;

                    // controls
                    case IDC_BTN_TEST:
                        ping(hwnd);
                        break;

                    case IDC_BTN_DNS:
                        dns(hwnd);
                        break;

                    case IDC_BTN_ATTACK:
                        if(!g_bAttack) // start attack
                        {
                            g_bAttack = TRUE;
                            setup_attack(hwnd);
                        }
                        else // stop unicorns
                        {
                            g_bAttack = FALSE;
                            SetWindowText(hBtnAttack, "Attack");
                            ShowWindow(hIcoLeftA, FALSE);
                            ShowWindow(hIcoRightA, FALSE);
                            ShowWindow(hIcoLeftI, TRUE);
                            ShowWindow(hIcoRightI, TRUE);
                            EnableWindow(hBtnTest, TRUE);
                            EnableWindow(hBtnDNS, TRUE);
                            EnableWindow(hBtnResetData, TRUE);

                            WSACleanup();
                        }
                        break;

                    case IDC_BTN_RESETDATA:
                        SetWindowText(hEditDataSent, "0");
                        g_nDataSent = 0;
                        break;

                    case IDC_BTN_MUSIC:
                        if(GetWindowLongPtr(hBtnMusic, GWL_USERDATA) == 1)
                        {
                            stop_music();
                            SetWindowLongPtr(hBtnMusic, GWL_USERDATA, 0);
                        }
                        else
                            play_music(hBtnMusic);
                        break;

                    default:
                        break;
                }
            }
            return 0;

        case WM_HSCROLL:
            if(LOWORD(wParam) == TB_THUMBTRACK)
            {
                char szBuffer[32] = {0};
                DWORD dwPos = SendMessage(hTbSize, TBM_GETPOS, 0, 0);

                wsprintf(szBuffer, "%ld.0", dwPos);
                SetWindowText(hEditSize, szBuffer);
            }
            return 0;

        case WM_TRAYICON:
            if(lParam == WM_LBUTTONDBLCLK)
            {
                if(wParam == IDR_TRAYICON)
                {
                    // remove tray icon
                    NOTIFYICONDATA  nid;
                    HWND            hChild;

                    nid.cbSize  = sizeof(NOTIFYICONDATA);
                    nid.hWnd    = hwnd;
                    nid.uID     = IDR_TRAYICON;
                    Shell_NotifyIcon(NIM_DELETE, &nid);

                    if((hChild = FindWindow(WC_AC, NULL)) != NULL)
                        ShowWindow(hChild, SW_SHOWNORMAL);
                    if((hChild = FindWindow(WC_PS, NULL)) != NULL)
                        ShowWindow(hChild, SW_SHOWNORMAL);
                    if((hChild = FindWindow(NULL, "Network Info")) != NULL)
                            ShowWindow(hChild, SW_SHOWNORMAL);
                    if((hChild = FindWindow(NULL, "Filter Port")) != NULL)
                        ShowWindow(hChild, SW_SHOWNORMAL);

                    ShowWindow(hwnd, SW_SHOWNORMAL);
                    SetForegroundWindow(hwnd);
                }
            }
            if(lParam == WM_RBUTTONDOWN)
            {
                if(wParam == IDR_TRAYICON)
                {
                    HMENU hMenu = CreatePopupMenu();
                    POINT p;

                    GetCursorPos(&p);

                    AppendMenu(hMenu, MF_STRING, IDR_TRAY_SHOW, "Show UDP Unicorn");
                    AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
                    AppendMenu(hMenu, MF_STRING, IDR_TRAY_CLOSE, "Close");
                    SetMenuDefaultItem(hMenu, IDR_TRAY_SHOW, FALSE);

                    SetForegroundWindow(hwnd);

                    TrackPopupMenuEx(hMenu,
                                     TPM_LEFTALIGN | TPM_LEFTBUTTON,
                                     p.x,
                                     p.y,
                                     hwnd,
                                     NULL);
                }
            }
            return 0;

        case WM_SYSCOMMAND:
            switch(wParam)
            {
                case SC_MINIMIZE:
                    {
                        if(GetWindowLongPtr(hwnd, GWL_USERDATA) == MIN_SHOW)
                            return DefWindowProc(hwnd, msg, wParam, lParam);

                        NOTIFYICONDATA  nid;
                        HWND            hChild;

                        nid.cbSize  = sizeof(NOTIFYICONDATA);
                        nid.hWnd    = hwnd;
                        nid.uID     = IDR_TRAYICON;
                        nid.uFlags  = NIF_MESSAGE | NIF_ICON | NIF_TIP;
                        nid.uCallbackMessage = WM_TRAYICON;
                        nid.hIcon   = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAIN));
                        strcpy(nid.szTip, "UDP Unicorn");
                        Shell_NotifyIcon(NIM_ADD, &nid);

                        ShowWindow(hwnd, SW_HIDE);

                        if((hChild = FindWindow(WC_AC, NULL)) != NULL)
                            ShowWindow(hChild, SW_HIDE);
                        if((hChild = FindWindow(WC_PS, NULL)) != NULL)
                            ShowWindow(hChild, SW_HIDE);
                        if((hChild = FindWindow(NULL, "Network Info")) != NULL)
                            ShowWindow(hChild, SW_HIDE);
                        if((hChild = FindWindow(NULL, "Filter Port")) != NULL)
                            ShowWindow(hChild, SW_HIDE);
                    }
                    return TRUE;
            }
            return 0;

        case WM_CLOSE:
            save_config();
            EndDialog(hwnd, 0);
            return 0;

        default:
            return 0;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst,
                     LPSTR lpszArgument, int nFunsterStil)
{
    return DialogBox(hInst, MAKEINTRESOURCE(DLG_MAIN), NULL, DlgMain);
}


