
 METASOURCES = AUTO
 LIBS =
 libcricketp2pclientIncludedir = /usr/local/include/libjingle/talk/p2p/client

 # the library search path.
 libcricketp2pclientdir = /usr/local/lib/libjingle

 libcricketp2pclient_so_LDFLAGS = -Wl,-soname,libcricketp2pclient.so -shared \
 -L/usr/local/lib/ -L/usr/local/lib/libjingle

 libcricketp2pclient_PROGRAMS = libcricketp2pclient.so

 libcricketp2pclient_so_SOURCES =  basicportallocator.cc \
			          httpportallocator.cc \
                                  socketmonitor.cc

 libcricketp2pclientInclude_HEADERS =    basicportallocator.h \
                                  socketmonitor.h \
                                  sessionmanagertask.h \
                                  sessionsendtask.h \
				  httpportallocator.h

 AM_CPPFLAGS = -I../../.. -DLINUX -DPOSIX -DINTERNAL_BUILD

