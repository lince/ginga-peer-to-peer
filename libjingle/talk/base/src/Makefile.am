
AM_CXXFLAGS = -DPOSIX
INCLUDES = -I$(top_srcdir) -I/usr/local/include/ $(all_includes) 

# the library search path.
libcricketbasedir = /usr/local/lib/libjingle

libcrickettestdir = /usr/local/lib/libjingle

libcricketbase_so_LDFLAGS = -Wl,-soname,libcricketbase.so -shared \
-L/usr/local/lib/ -L/usr/local/lib/libjingle -L./

libcrickettest_so_LDFLAGS = -Wl,-soname,libcrickettest.so -shared \
-L/usr/local/lib/ -L/usr/local/lib/libjingle -L./


libcricketbase_PROGRAMS = libcricketbase.so libcrickettest.so

libcrickettest_PROGRAMS = natserver nat_unittest virtualsocket_unittest

natserver_SOURCES = natserver_main.cc
natserver_LDADD = libcrickettest.so libcricketbase.so -lpthread -lssl
nat_unittest_SOURCES = nat_unittest.cc
nat_unittest_LDADD = libcrickettest.so libcricketbase.so -lpthread -lssl
virtualsocket_unittest_SOURCES = virtualsocket_unittest.cc
virtualsocket_unittest_LDADD = libcrickettest.so libcricketbase.so -lpthread -lssl

libcricketbase_so_SOURCES = socketaddress.cc \
                            time.cc \
                            asyncudpsocket.cc \
                            messagequeue.cc \
                            thread.cc \
                            physicalsocketserver.cc \
                            bytebuffer.cc \
                            asyncpacketsocket.cc \
                            network.cc \
                            asynctcpsocket.cc \
                            socketadapters.cc \
                            md5c.c \
                            base64.cc \
                            task.cc \
                            taskrunner.cc \
                            host.cc \
                            proxydetect.cc \
                            socketaddresspair.cc \
                            stringencode.cc \
                            stringdigest.cc \
                            stringutils.cc \
                            proxyinfo.cc \
                            common.cc \
                            logging.cc \
                            stream.cc \
                            ssladapter.cc \
                            openssladapter.cc \
                            helpers.cc \
                            asynchttprequest.cc \
                            firewallsocketserver.cc \
                            httpcommon.cc \
                            httpbase.cc \
                            httpclient.cc \
			    httpserver.cc \
                            socketpool.cc \
                            signalthread.cc \
                            autodetectproxy.cc \
                            urlencode.cc \
                            pathutils.cc \
			    fileutils.cc \
			    unixfilesystem.cc \
			    tarstream.cc \
			    streamutils.cc \
			    diskcache.cc \
			    diskcachestd.cc

libcrickettest_so_SOURCES = testclient.cc \
                           natserver.cc \
                           natsocketfactory.cc \
                           nattypes.cc \
                           virtualsocketserver.cc

