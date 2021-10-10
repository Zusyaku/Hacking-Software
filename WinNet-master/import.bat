:: GitHub : https://github.com/niirmaaltwaatii/WinNet
REM BY N11rm44L 7w44711

echo {
set /p iprt="Specific or From exports/ (s/e) :"
if %iprt%==e (goto exports)
if %iprt%==s (goto specific)

:exports
netsh wlan add profile filename=exports/Wi-Fi-*
goto end

:specific
set /p fn="File Location/Name : "
netsh wlan add profile filename=%fn%

:end

echo }

pause

winnet.bat