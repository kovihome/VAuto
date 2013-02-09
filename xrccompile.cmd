rem
rem VAuto XRC compiler batch
rem
echo VAuto XRC compiler batch

rem
rem First create a cpp file from xrc and the embedded images
rem Additionally create a header file for resource classes
rem
c:\Dev\wx2.8.10\utils\wxrc\vc_mswd\wxrc /v /c /e /o src\VAutoResources.cpp res/VAuto.xrc

rem
rem Next create a cpp file with strings in xrc resource for i18n (poedit)
rem
c:\Dev\wx2.8.10\utils\wxrc\vc_mswd\wxrc /v /g /o src\XrcStrings.cpp res\VAuto.xrc
