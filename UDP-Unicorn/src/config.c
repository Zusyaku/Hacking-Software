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

#include "config.h"
#include "ac.h"

extern int  g_nPortStart,
            g_nPortEnd,
            g_nConnections,
            g_nMaxConnections;
extern UINT g_filter_port;

//////////////////////////////////////////////////////////////////////////////

/* delete any existing config.ini and create a new one */
static int new_config(void)
{
    FILE* fp = NULL;

    // see if it exists first
    if(GetFileAttributes(CONFIG_FILE) == ERROR_SUCCESS)
    {
        SetFileAttributes(CONFIG_FILE, FILE_ATTRIBUTE_NORMAL);
        if(DeleteFile(CONFIG_FILE) == 0)
        {
            err("Unable to delete config file for new creation");
            return 0;
        }
    }

    CreateDirectory("data", NULL);

    // now create a new file
    fp = fopen(CONFIG_FILE, "w+");
    if(fp == NULL)
    {
        err("Unable to create new config file, increase your privileges");
        return 0;
    }

    fclose(fp);
    save_config();
    return 1;
}

//////////////////////////////////////////////////////////////////////////////

/* parse the config.ini file */
void read_config(void)
{
    FILE*   fp          = NULL;
    char    buf[1024]   = {0};
    int     i;

    if((fp = fopen(CONFIG_FILE, "r")) != NULL)
    {
        // parse the configurations
        for(;fscanf(fp, "%s", buf) != EOF;)
        {
            if(strcmp(buf, "[Options]") == 0)
            {
                memset(buf, 0, sizeof(buf));

                for(;; fscanf(fp, "%s", buf))
                {
                    if(sscanf(buf, "top=%d", &i) > 0)
                    {
                        if(i == 1)
                        {
                            SetWindowPos(g_hUnicorn, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                            CheckMenuItem(GetMenu(g_hUnicorn), IDR_OP_TOP, MF_CHECKED);
                        }
                    }
                    else if(sscanf(buf, "hide=%d", &i) > 0)
                    {
                        if(i == 1)
                        {
                            SetWindowLongPtr(g_hUnicorn, GWL_USERDATA, MIN_HIDE);
                            CheckMenuItem(GetMenu(g_hUnicorn), IDR_OP_MIN_TRAY, MF_CHECKED);
                        }
                    }

                    if(buf[0] == '[')
                        break;
                }
            }

            if(strcmp(buf, "[ActiveConnections]") == 0)
            {
                memset(buf, 0, sizeof(buf));

                fscanf(fp, "%s", buf);

                if(sscanf(buf, "filter=%d", &i) > 0)
                    g_filter_port = i;
            }

            if(strcmp(buf, "[Portscan]") == 0)
            {
                memset(buf, 0, sizeof(buf));

                for(;; fscanf(fp, "%s", buf))
                {
                    if(sscanf(buf, "start=%d", &i) > 0)
                        g_nPortStart = i;
                    else if(sscanf(buf, "end=%d", &i) > 0)
                        g_nPortEnd = i;
                    else if(sscanf(buf, "max_connections=%d", &i) > 0)
                        g_nMaxConnections = i;

                    if(buf[0] == '[')
                        break;
                }
            }

            if(strcmp(buf, "[Attack]") == 0)
            {
                memset(buf, 0, sizeof(buf));

                for(;fscanf(fp, "%s", buf) != EOF;)
                {
                    char tmp[32] = {0};

                    if(sscanf(buf, "delay=%d", &i) > 0)
                    {
                        sprintf(tmp, "%d", i);
                        SetDlgItemText(g_hUnicorn, IDC_EDT_DELAY, tmp);
                    }
                    else if(sscanf(buf, "threads=%d", &i) > 0)
                    {
                        sprintf(tmp, "%d", i);
                        SetDlgItemText(g_hUnicorn, IDC_EDT_THREADS, tmp);
                    }
                    else if(sscanf(buf, "sockets=%d", &i) > 0)
                    {
                        sprintf(tmp, "%d", i);
                        SetDlgItemText(g_hUnicorn, IDC_EDT_SOCKETS, tmp);
                    }

                    if(buf[0] == '[')
                        break;
                }
            }
        }

        fclose(fp);
    }
    else
    {
        if(new_config())
            save_config();
    }
}

//////////////////////////////////////////////////////////////////////////////

/* write a new config.ini file */
void save_config(void)
{
    MENUITEMINFO item   = {0};
    HMENU   hMenu       = GetMenu(g_hUnicorn);
    HWND    hDelay      = GetDlgItem(g_hUnicorn, IDC_EDT_DELAY);
    HWND    hThreads    = GetDlgItem(g_hUnicorn, IDC_EDT_THREADS);
    HWND    hSockets    = GetDlgItem(g_hUnicorn, IDC_EDT_SOCKETS);
    FILE*   fp          = NULL;
    char    buf[1024]   = {0};
    int     top = 0, hide = 0, delay, threads, sockets, len;

    item.cbSize = sizeof(MENUITEMINFO);
    item.fMask  = MIIM_STATE;

    // get the data
    if(GetMenuItemInfo(hMenu, IDR_OP_TOP, FALSE, &item))
        if(item.fState == MFS_CHECKED)
            top = 1;

    if(GetMenuItemInfo(hMenu, IDR_OP_MIN_TRAY, FALSE, &item))
        if(item.fState == MFS_CHECKED)
            hide = 1;

    len = GetWindowTextLength(hDelay);
    if(len <= 0)
        delay = 10;
    else
    {
        GetWindowText(hDelay, buf, len + 1);
        sscanf(buf, "%d", &delay);
    }

    len = GetWindowTextLength(hThreads);
    if(len <= 0)
        threads = 1;
    else
    {
        GetWindowText(hThreads, buf, len + 1);
        sscanf(buf, "%d", &threads);
    }

    len = GetWindowTextLength(hSockets);
    if(len <= 0)
        sockets = 1;
    else
    {
        GetWindowText(hSockets, buf, len + 1);
        sscanf(buf, "%d", &sockets);
    }

    CreateDirectory("data", NULL);

    // clear any existing config file
    SetFileAttributes(CONFIG_FILE, FILE_ATTRIBUTE_NORMAL);
    DeleteFile(CONFIG_FILE);

    if((fp = fopen(CONFIG_FILE, "w+")) != NULL)
    {
        fprintf(fp, "[Options]\ntop=%d\nhide=%d\n\n"
                    "[ActiveConnections]\nfilter=%d\n\n"
                    "[Portscan]\nstart=%d\nend=%d\nmax_connections=%d\n\n"
                    "[Attack]\ndelay=%d\nthreads=%d\nsockets=%d\n\n",
                    top, hide,
                    g_filter_port,
                    g_nPortStart, g_nPortEnd, g_nMaxConnections,
                    delay, threads, sockets);

        fclose(fp);
    }
}
