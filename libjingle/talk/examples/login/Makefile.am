# set the include path found by configure
 INCLUDES = -I/usr/local/include/ \
 	$(all_includes)

libcricketexamplesloginIncludedir = /usr/local/include/libjingle/talk/examples/login

libcricketexamplesloginInclude_HEADERS = xmppauth.h \
                 xmpppump.h \
                 xmppsocket.h \
                 xmppthread.h \
                 jingleinfotask.h \
                 presenceouttask.h \
                 presencepushtask.h \
                 status.h

libcricketexampleslogin_PROGRAMS = libcricketexampleslogin.so
libcricketexampleslogindir = /usr/local/lib/libjingle/


libcricketexampleslogin_so_LDFLAGS = -shared \
    					-Wl,-soname,libcricketexampleslogin.so \
    					-L/usr/local/lib -L/usr/local/lib/libjingle -L../thread -L./ 
				

# libcricketexampleslogin_so_LDADD =  $(top_srcdir)/talk/xmpp/libcricketxmpp.so \
#	       		$(top_srcdir)/talk/xmllite/libcricketxmllite.so \
#	      	 	$(top_srcdir)/talk/base/src/libcricketbase.so \
#	      		$(EXPAT_LIBS) -lpthread -lssl -lcrypto

libcricketexampleslogin_so_SOURCES = xmppsocket.cc \
				     xmppauth.cc \
				     xmppthread.cc \
				     xmpppump.cc  \
				     jingleinfotask.cc \
				     presenceouttask.cc \
				     presencepushtask.cc


# XMLLIBS = ../../xmllite/libcricketxmllite.so 

noinst_PROGRAMS = login

login_CXXFLAGS = $(AM_CXXFLAGS)

login_SOURCES = login_main.cc xmppsocket.cc xmppthread.cc xmpppump.cc xmppauth.cc

login_LDADD =  $(top_srcdir)/talk/xmpp/libcricketxmpp.so \
	       $(top_srcdir)/talk/xmllite/libcricketxmllite.so \
	       $(top_srcdir)/talk/base/src/libcricketbase.so \
	       $(EXPAT_LIBS) -lpthread -lssl -lcrypto
AM_CPPFLAGS  = -DPOSIX
