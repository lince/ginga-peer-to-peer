METASOURCES = AUTO
LIBS =

libcricketsessionfileshareIncludedir = /usr/local/include/libjingle/talk/session/fileshare

libcricketsessionfileshareInclude_HEADERS = fileshare.h 

#set the include path found by configure
INCLUDES = -I/usr/local/include/  \
	$(all_includes)


libcricketsessionfilesharedir = /usr/local/lib/libjingle

libcricketsessionfileshare_so_SOURCES = fileshare.cc

libcricketsessionfileshare_so_LDFLAGS = -shared \
    -Wl,-soname,libcricketsessionfileshare.so \
    -L/usr/local/lib -L/usr/local/lib/libjingle

libcricketsessionfileshare_PROGRAMS = libcricketsessionfileshare.so



AM_CPPFLAGS = -DPOSIX 
#noinst_LTLIBRARIES = libcricketsessionfileshare.la
				    
