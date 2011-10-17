METASOURCES = AUTO
LIBS =

libcricketsessiontunnelIncludedir = /usr/local/include/libjingle/talk/session/tunnel

libcricketsessiontunnelInclude_HEADERS = 	tunnelsessionclient.h \
 			                pseudotcpchannel.h

#set the include path found by configure
INCLUDES = -I/usr/local/include/ -I/usr/local/include/ginga \
	$(all_includes)


libcricketsessiontunneldir = /usr/local/lib/libjingle

libcricketsessiontunnel_so_LDFLAGS = -shared \
    -Wl,-soname,libcricketsessiontunnel.so \
    -L/usr/local/lib -L/usr/local/lib/libjingle

libcricketsessiontunnel_PROGRAMS = libcricketsessiontunnel.so


				    
libcricketsessiontunnel_so_SOURCES = tunnelsessionclient.cc \
                                     pseudotcpchannel.cc
#noinst_LTLIBRARIES = libcricketsessiontunnel.la

AM_CXXFLAGS = -DPOSIX
