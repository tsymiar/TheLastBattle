
# Tweak these for your system
OPENSSLINC=..\..\openssl-1.0.2-beta3\include
OPENSSLLIB=..\..\..\Lib\x64

!if "$(TARGET)" == ""
TARGET=Release
!endif

!if "$(TARGET)" == "Debug"
SUFFIX=_debug
CPPFLAGS=/Od /MDd
DLLFLAGS=/DEBUG /LDd
!else
CPPFLAGS=/Og /Oi /O2 /Oy /GF /Y- /MD /DNDEBUG
DLLFLAGS=/DEBUG /LD
!endif

CPPFLAGS=/nologo /GL /Zi /EHsc $(CPPFLAGS) /Iwin32 /Iinclude /I$(OPENSSLINC) $(ZLIBINC) -DLIBSSH2_WIN32
CFLAGS=$(CPPFLAGS)
RCFLAGS=/Iinclude
DLLFLAGS=$(CFLAGS) $(DLLFLAGS)
LIBS=$(OPENSSLLIB)\libeay32.lib $(OPENSSLLIB)\ssleay32.lib ws2_32.lib user32.lib

INTDIR=$(TARGET)\$(SUBDIR)


