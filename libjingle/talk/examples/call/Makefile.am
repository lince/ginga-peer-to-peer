EXTRA_DIST=call.vcproj
bin_PROGRAMS = call 
call_CXXFLAGS = $(AM_CXXFLAGS)
call_SOURCES = call_main.cc callclient.cc console.cc 
callIncludedir = /usr/local/include/libjingle/talk/examples/call
callInclude_HEADERS = callclient.h  console.h 
call_LDADD =  \
	       $(top_srcdir)/talk/examples/login/libcricketexampleslogin.so \
	       $(top_srcdir)/talk/session/phone/libcricketsessionphone.so \
	       $(top_srcdir)/talk/p2p/client/libcricketp2pclient.so \
	       $(top_srcdir)/talk/p2p/base/libcricketp2pbase.so \
	       $(top_srcdir)/talk/xmpp/libcricketxmpp.so \
	       $(top_srcdir)/talk/xmllite/libcricketxmllite.so \
	       $(top_srcdir)/talk/base/libcricketbase.so \
	       $(EXPAT_LIBS) $(ORTP_LIBS) -lpthread -lssl -lcrypto  $(ILBC_LIBS) $(SPEEX_LIBS) $(GLIB_LIBS) $(MEDIA_LIBS) 
AM_CPPFLAGS  = -DPOSIX
