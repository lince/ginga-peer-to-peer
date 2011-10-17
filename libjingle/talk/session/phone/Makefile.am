METASOURCES = AUTO
LIBS =

libcricketsessionphoneIncludedir = /usr/local/include/libjingle/talk/session/phone

libcricketsessionphone_HEADERS =    audiomonitor.h \
				    channelmanager.h \
				    linphonemediaengine.h \
				    mediaengine.h \
				    phonesessionclient.h \
				    voicechannel.h \
                                    call.h \
				    mediachannel.h  \
				    codec.h

#set the include path found by configure
INCLUDES = -I/usr/local/include/ -I/usr/local/include/ginga \
	$(all_includes)


libcricketsessionphonedir = /usr/local/lib/libjingle

libcricketsessionphone_so_LDFLAGS = -shared \
    -Wl,-soname,libcricketsessionphone.so \
    -L/usr/local/lib -L/usr/local/lib/libjingle

libcricketsessionphone_PROGRAMS = libcricketsessionphone.so


EXTRA_DIST=gipslitemediaengine.cc gipslitemediaengine.h
if GIPS
nodist_libcricketsessionphone_so_SOURCES=gipsmediaengine.cc \
					 gipsstatsmonitor.cc
else
dist_libcricketsessionphone_so_SOURCES=linphonemediaengine.cc
endif

libcricketsessionphone_so_SOURCES = audiomonitor.cc \
				    channelmanager.cc \
				    voicechannel.cc \
				    call.cc \
				    phonesessionclient.cc 


AM_CPPFLAGS := -DPOSIX $(ORTP_CFLAGS) $(ILBC_CFLAGS) -I$(top_srcdir)/talk/third_party/mediastreamer $(GLIB_CFLAGS)
#noinst_LTLIBRARIES = libcricketsessionphone.la
				    
