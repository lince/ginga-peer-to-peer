bin_PROGRAMS = pcp 
pcp_CXXFLAGS = $(AM_CXXFLAGS)
pcp_SOURCES = pcp_main.cc
pcp_LDADD =  $(top_srcdir)/talk/session/fileshare/libcricketsessionfileshare.so \
             $(top_srcdir)/talk/examples/login/libcricketexampleslogin.so \
	     $(top_srcdir)/talk/session/tunnel/libcricketsessiontunnel.so \
             $(top_srcdir)/talk/p2p/client/libcricketp2pclient.so \
             $(top_srcdir)/talk/p2p/base/libcricketp2pbase.so \
             $(top_srcdir)/talk/xmpp/libcricketxmpp.so \
             $(top_srcdir)/talk/xmllite/libcricketxmllite.so \
             $(top_srcdir)/talk/base/src/libcricketbase.so \
             $(top_srcdir)/talk/base/src/libcrickettest.so \
             $(EXPAT_LIBS) -lpthread -lssl -lcrypto

AM_CPPFLAGS = -DPOSIX
EXTRA_DIST = pcp.vcproj
