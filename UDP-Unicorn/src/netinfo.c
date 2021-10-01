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

#include "netinfo.h"

extern BOOL g_bAttack;

/* write to socket */
static int net_write(SOCKET sock, char* buffer, int size)
{
    int written = 0,
        ret     = 0;

    if(size == -1)
        size = strlen(buffer);

    while(written < size)
    {
        ret = send(sock, buffer, (size - written), 0);
        written += ret;
        buffer += ret;

        if(ret < 1)
            return FALSE;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

/* read data from socket */
static int read_timeout(SOCKET sock, char* buffer, unsigned int size, unsigned int timeout)
{
    struct timeval  tv;
    fd_set          fds;
    int             ret;

    if(timeout != 0)
    {
        // convert timeout to seconds and microseconds
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        FD_ZERO(&fds);
        FD_SET((unsigned int)sock, &fds);
        ret = select(sock + 1, &fds, NULL, NULL, &tv);
        if(ret < 1)
        {
            fprintf(stderr, "select() failed: %d", WSAGetLastError());
            return FALSE;
        }
    }

    ret = recv(sock, buffer, size, 0);
    if(ret < 0)
    {
        fprintf(stderr, "recv() failed: %d", WSAGetLastError());
        return FALSE;
    }
    else if(ret == 0)
        return FALSE;

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

/* get URL source */
static int retrieve_source(char* host, FILE* fp)
{
    #define fail()\
    {\
        WSACleanup();\
        return FALSE;\
    }

    const int   HTTP_GET_SIZE = 25;
    char        buffer[1024] = {0};
    char*       http_request;
    int         ret = 0;

    SOCKET      sock = INVALID_SOCKET;

    struct addrinfo     *serv = NULL;
    struct addrinfo     hints;
    struct sockaddr_in  *sin;

    //--------------------------------------//

    http_request = calloc(strlen(host) + HTTP_GET_SIZE + 256, sizeof(char));
    if(http_request == NULL)
    {
        fprintf(stderr, "ERROR: http_request = NULL\n");
        fail();
    }

    wsprintf(http_request, "GET %s HTTP/1.1\r\nhost: %s\r\n\r\n", "/", host);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family     = AF_UNSPEC;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_flags      = AI_PASSIVE;

    // retrieve information about host
    ret = getaddrinfo(host, "http", &hints, &serv);
    if(ret != 0)
    {
        fprintf(stderr, "ERROR: getaddrinfo: %d", WSAGetLastError());
        fail();
    }

    // setup socket
    sin = (struct sockaddr_in*)serv->ai_addr;
    sock = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol);
    if(sock == -1)
    {
        fprintf(stderr, "socket() failed: %d", WSAGetLastError());
        freeaddrinfo(serv);
        fail();
    }

    // connect to server
    ret = connect(sock, serv->ai_addr, serv->ai_addrlen);
    if(ret != 0)
    {
        fprintf(stderr, "connect() failed: %d", WSAGetLastError());
        freeaddrinfo(serv);
        fail();
    }

    freeaddrinfo(serv);

    //---send get request---//
    ret = net_write(sock, http_request, -1);
    free(http_request);
    if(!ret)
    {
        fail();
    }

    // loop until finished receiving all data
    while((ret = read_timeout(sock, buffer, 1024, 1000)) > 0)
    {
        fwrite(buffer, sizeof(buffer), ret, fp);
    }
    fflush(fp);

    if(ret == -1)
        fail();

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

/* retrieve the external IP address */
static void get_public_ip(char* public_ip)
{
    FILE*   fp = fopen(TEMP_FILE, "w+");
    char    buf[1024] = {0};
    int     i;

    if(fp != NULL)
    {
        // try some websites to get the public IP address
        if(retrieve_source("checkip.dyndns.com", fp))
        {
            fclose(fp);
            fp = fopen(TEMP_FILE, "r");

            while(!feof(fp))
            {
                fgets(buf, sizeof(buf), fp);

                if(strncmp(buf, "<html><head><title>Current IP Check</title></head><body>Current IP Address:", 75) == 0)
                {
                    sscanf(buf, "<html><head><title>Current IP Check</title></head><body>Current IP Address: %s</body></html>",
                            public_ip);
                }
            }
        }
        else if(retrieve_source("ipswift.com", fp))
        {
            fclose(fp);
            fp = fopen(TEMP_FILE, "r");

            while(!feof(fp))
            {
                fgets(buf, sizeof(buf), fp);

                if(strncmp(buf, "<h2>Your", 8) == 0)
                {
                    sscanf(buf, "<h2>Your IP: %s</h2>", public_ip);
                }
            }
        }
        else if(retrieve_source("www.formyip.com", fp))
        {
            fclose(fp);
            fp = fopen(TEMP_FILE, "r");

            while(!feof(fp))
            {
                fgets(buf, sizeof(buf), fp);

                if(strncmp(buf, "<title>My ip address is", 23) == 0)
                {
                    sscanf(buf, "<title>My ip address is %s", public_ip);
                }
            }
        }
        else if(retrieve_source("www.cmyip.com", fp))
        {
            fclose(fp);
            fp = fopen(TEMP_FILE, "r");

            while(!feof(fp))
            {
                fgets(buf, sizeof(buf), fp);

                if(strncmp(buf, "<title>My", 9) == 0)
                {
                    sscanf(buf, "<title>My IP is %s", public_ip);
                }
            }
        }
        else
            strcpy(public_ip, "???");
    }
    else
        strcpy(public_ip, "???");

    fclose(fp);
    DeleteFile(TEMP_FILE);

    // remove any non-IP address related characters
    //  (any invalid characters will only be after the address)
    for(i=strlen(public_ip); i>0; --i)
    {
        if(!isdigit(public_ip[i]) && public_ip[i] != '.')
            public_ip[i] = '\0';
    }
}

//////////////////////////////////////////////////////////////////////////////

/* send ARP request to IP address to retrieve MAC address */
static BOOL resolve_mac(const char* dst, char* mac)
{
    IPAddr dst_ip = 0;
    IPAddr src_ip = 0;
    struct hostent* localhost;
    ULONG u_mac[2];
    ULONG len = 6;
    char szLocalhost[256] = {0};
    LPBYTE mac_addr;
    DWORD ret;

    // find local IP address
    gethostname(szLocalhost, sizeof(szLocalhost));
    localhost = gethostbyname(szLocalhost);
    memcpy(&src_ip, localhost->h_addr_list[0], sizeof(struct in_addr));

    // convert char* to destination struct
    dst_ip = inet_addr(dst);

    // send the packet
    ret = SendARP(dst_ip, src_ip, (PULONG)&u_mac, &len);
    if(ret == NO_ERROR)
    {
        if(!mac) return TRUE;

        if(len)
        {
            // write the MAC address to the pointer
            char buf[8] = {0};
            int i;
            mac_addr = (LPBYTE)&u_mac;

            for(i=0; i<(int)len; ++i)
                if(i == (len - 1))
                {
                    wsprintf(buf, "%.2X", (int)mac_addr[i]);
                    strcat(mac, buf);
                }
                else
                {
                    wsprintf(buf, "%.2X:", (int)mac_addr[i]);
                    strcat(mac, buf);
                }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

/* retrieve various information about the computer's network */
static void net_info(HWND hwnd)
{
    struct  hostent* localhost;
    struct  in_addr  localip;
    char*   private_ip      = 0;
    char    public_ip[32]   = {0};
    char    hostname[256]   = {0};
    char    mac[32]         = {0};

    if(!g_bAttack)
        if(!start_winsock())
            return;

    // get local host name
    if(gethostname(hostname, sizeof(hostname)) == 0)
    {
        SetDlgItemText(hwnd, IDC_NETINFO_EDT1, hostname);

        // get IP address of local host
        localhost = gethostbyname(hostname);
        memcpy(&localip, localhost->h_addr_list[0], sizeof(struct in_addr));
        private_ip = inet_ntoa(localip);

        SetDlgItemText(hwnd, IDC_NETINFO_EDT2, private_ip);

        // get MAC address of localhost
        if(!resolve_mac(private_ip, mac))
            strcpy(mac, "???");

        SetDlgItemText(hwnd, IDC_NETINFO_EDT4, mac);

        // get public IP address
        get_public_ip(public_ip);

        SetDlgItemText(hwnd, IDC_NETINFO_EDT3, public_ip);
    }
    else
        err("Failed to retrieve local hostname, please restart the program and try again");

    if(!g_bAttack)
        WSACleanup();
}

//////////////////////////////////////////////////////////////////////////////

/* avoid a pause in the main event loop */
VOID WINAPI _net_info(LPVOID lpBuffer)
{
    HWND hwnd = (HWND)lpBuffer;

    net_info(hwnd);

    ExitThread(0);
}

//////////////////////////////////////////////////////////////////////////////

/* net info procedure */
BOOL CALLBACK NetInfoProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_INITDIALOG:
            SetDlgItemText(hwnd, IDC_NETINFO_EDT1, "Acquiring...");
            SetDlgItemText(hwnd, IDC_NETINFO_EDT2, "Acquiring...");
            SetDlgItemText(hwnd, IDC_NETINFO_EDT3, "Acquiring...");
            SetDlgItemText(hwnd, IDC_NETINFO_EDT4, "Acquiring...");

            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&_net_info, hwnd, 0, 0);
            return 0;

        case WM_CLOSE:
            EndDialog(hwnd, 0);
            return 0;

        default:
            return 0;
    }

    return TRUE;
}
