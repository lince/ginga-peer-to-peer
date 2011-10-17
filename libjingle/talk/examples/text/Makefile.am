EXTRA_DIST=text.vcproj
bin_PROGRAMS = text
text_CXXFLAGS = $(AM_CXXFLAGS)
text_SOURCES = text_main.cc textclient.cc console.cc 
textIncludedir = /usr/local/include/libjingle/talk/examples/text
textInclude_HEADERS = textclient.h  console.h 
text_LDADD =  \
	       $(top_srcdir)/talk/examples/login/libcricketexampleslogin.so \
	       $(top_srcdir)/talk/session/phone/libcricketsessionphone.so \
	       $(top_srcdir)/talk/p2p/client/libcricketp2pclient.so \
	       $(top_srcdir)/talk/p2p/base/libcricketp2pbase.so \
	       $(top_srcdir)/talk/xmpp/libcricketxmpp.so \
	       $(top_srcdir)/talk/xmllite/libcricketxmllite.so \
	       $(top_srcdir)/talk/base/libcricketbase.so \
	       $(EXPAT_LIBS) $(ORTP_LIBS) -lpthread -lssl -lcrypto  $(ILBC_LIBS) $(SPEEX_LIBS) $(GLIB_LIBS) $(MEDIA_LIBS) 
AM_CPPFLAGS  = -DPOSIX
