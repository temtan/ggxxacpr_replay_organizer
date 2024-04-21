
rc.exe japanese.rc
link.exe /DLL /NOENTRY /MACHINE:X64 japanese.res

rc.exe english.rc
link.exe /DLL /NOENTRY /MACHINE:X64 english.res

rc.exe korean.rc
link.exe /DLL /NOENTRY /MACHINE:X64 korean.res

rc.exe chinese_traditional.rc
link.exe /DLL /NOENTRY /MACHINE:X64 chinese_traditional.res
