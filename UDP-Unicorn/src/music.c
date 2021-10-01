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

#include "music.h"

/* search the data directory for the music file */
static int find_music_file(char* file)
{
    HANDLE hFind;
    WIN32_FIND_DATA fd = {0};
    char buf[MAX_PATH] = {0};

    GetCurrentDirectory(sizeof(buf), buf);
    strcat(buf, "\\data\\*.*");

    hFind = FindFirstFile(buf, &fd);
    do
    {
        if(strncmp(fd.cFileName, "music", 5) == 0)
        {
            wsprintf(file, "data\\%s", fd.cFileName);
            FindClose(hFind);
            return 1;
        }
    }while(FindNextFile(hFind, &fd));

    MessageBox(NULL, "Unable to find the music file, make sure it's in the \"data\" folder and named \"music.mp3\" (file type may not matter)",
               0, MB_ICONWARNING);
    FindClose(hFind);
    return 0;
}

//////////////////////////////////////////////////////////////////////////////

/* play the music file with MCI */
void play_music(HWND hMusic)
{
    char buf[1024] = {0};
    char file[MAX_PATH] = {0};

    if(!find_music_file(file))
        return;

    wsprintf(buf, "open %s type mpegvideo alias music", file);

    mciSendString(buf, NULL, 0, 0);
    mciSendString("play music repeat", NULL, 0, 0);
    SetWindowLongPtr(hMusic, GWL_USERDATA, 1);
}

//////////////////////////////////////////////////////////////////////////////

/* close the music MCI resource */
void stop_music(void)
{
    mciSendString("close music", NULL, 0, 0);
}
