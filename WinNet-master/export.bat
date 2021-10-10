:: GitHub : https://github.com/niirmaaltwaatii/WinNet
REM BY N11rm44L 7w44711

echo {
set /p ex="ALL or Specific (a/s) :"
if %ex%==a (echo Exporting All Network Including Passwords ........ && goto all)
if %ex%==s (goto specific)

:all
netsh wlan export profile key=clear folder=exports
goto end

:specific
set /p exn="Name : "
netsh wlan export profile name=%exn% key=clear folder=exports

:end

echo }

pause

winnet.bat