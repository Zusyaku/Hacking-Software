:: https://github.com/niirmaaltwaatii/WinNet
REM N11rm44L 7w44711

echo {
set /p delopt="All or Specific [Choose Wisely , No Turning Back !] (A/s) :"
if %delopt%==A (netsh wlan delete profiles name=* interface=Wi-Fi)
if %delopt%==s (goto delspecific)
if %delopt%==S (goto delspecific)
echo }

:delspecific
set /p delnet="Network Name: "
netsh wlan delete profile name=%delnet% interface=Wi-Fi

echo }

pause

winnet.bat