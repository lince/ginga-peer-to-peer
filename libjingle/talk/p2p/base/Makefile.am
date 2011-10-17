METASOURCES = AUTO
LIBS =
 
libcricketp2pbaseIncludedir = /usr/local/include/libjingle/talk/p2p/base

AM_CPPFLAGS = -DPOSIX -DENABLE_DEBUG -D_DEBUG -g


# the library search path.
libcricketp2pbasedir = /usr/local/lib/libjingle

libcricketp2pbase_so_LDFLAGS = -Wl,-soname,libcricketp2pbase.so -shared \
 -L/usr/local/lib/ -L/usr/local/lib/libjingle/ \
 -L../../base/src/ -L../../xmllite/ -L../../xmpp/

libcricketp2pbase_PROGRAMS = libcricketp2pbase.so

P2PLIBS = ../../base/src/libcricketbase.so ../../base/src/libcrickettest.so -lpthread -lssl
XMLLIBS = ../../xmllite/libcricketxmllite.so ../../xmpp/libcricketxmpp.so $(EXPAT_LIBS)

libcricketp2pbase_so_LDADD = $(P2PLIBS) $(XMLLIBS)

bin_PROGRAMS = relayserver stunserver
noinst_PROGRAMS = stunserver_unittest session_unittest port_unittest

relayserver_SOURCES = relayserver.cc relayserver_main.cc
relayserver_LDADD = libcricketp2pbase.so $(P2PLIBS) $(XMLLIBS)
stunserver_SOURCES = stunserver.cc stunserver_main.cc
stunserver_LDADD = libcricketp2pbase.so $(P2PLIBS) $(XMLLIBS)
stunserver_unittest_SOURCES = stunserver_unittest.cc stunserver.cc
stunserver_unittest_LDADD =  libcricketp2pbase.so $(P2PLIBS) $(TESTLIBS) $(XMLLIBS)
session_unittest_SOURCES = session_unittest.cc stunserver.cc relayserver.cc
session_unittest_LDADD = libcricketp2pbase.so  $(TESTLIBS) $(P2PLIBS) $(XMLLIBS)
port_unittest_SOURCES = port_unittest.cc stunserver.cc relayserver.cc
port_unittest_LDADD =  libcricketp2pbase.so $(P2PLIBS) $(TESTLIBS) $(XMLLIBS) 


libcricketp2pbase_so_SOURCES = stun.cc \
			       port.cc \
			       udpport.cc \
			       tcpport.cc \
			       stunport.cc \
			       relayport.cc \
			       stunrequest.cc \
			       sessionmanager.cc \
			       session.cc \
			       transport.cc \
			       transportchannel.cc \
			       transportchannelproxy.cc \
			       p2ptransport.cc \
			       p2ptransportchannel.cc \
			       rawtransport.cc \
			       rawtransportchannel.cc \
			       constants.cc \
			       pseudotcp.cc

libcricketp2pbaseInclude_HEADERS =        candidate.h \
                               portallocator.h \
			       relayport.h \
		     	       session.h \
			       stunport.h \
			       tcpport.h \
			       port.h \
		       	       sessionid.h \
			       stunrequest.h \
			       udpport.h \
                               pseudotcp.h \
			       sessiondescription.h \
			       sessionmanager.h \
			       stun.h  \
			       relayserver.h \
			       stunserver.h \
			       sessionclient.h \
			       transport.h \
			       transportchannel.h \
			       transportchannelproxy.h \
			       transportchannelimpl.h \
			       p2ptransport.h \
			       p2ptransportchannel.h \
			       rawtransport.h \
			       rawtransportchannel.h \
			       constants.h \
			       common.h

