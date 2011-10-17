METASOURCES = AUTO
LIBS =

libcricketxmppIncludedir = /usr/local/include/libjingle/talk/xmpp

libcricketxmppInclude_HEADERS = asyncsocket.h \
			    	prexmppauth.h \
		    	    	saslhandler.h \
		      	    	xmpplogintask.h \
			    	jid.h \
		    	    	saslmechanism.h \
		      	    	xmppclient.h \
                            	constants.h \
		    	    	saslplainmechanism.h \
			    	xmppclientsettings.h \
			    	xmppstanzaparser.h \
		 	    	xmppengine.h \
		   	    	xmpptask.h \
			    	plainsaslhandler.h \
			    	saslcookiemechanism.h \
		     	    	xmppengineimpl.h \
			    	ratelimitmanager.h 

#set the include path found by configure
INCLUDES = -I/usr/local/include/ -I/usr/local/include/ginga \
	$(all_includes)


libcricketxmppdir = /usr/local/lib/libjingle

libcricketxmpp_so_LDFLAGS = -shared \
    -Wl,-soname,libcricketxmpp.so \
    -L/usr/local/lib -L/usr/local/lib/libjingle

libcricketxmpp_PROGRAMS = libcricketxmpp.so

libcricketxmpp_so_SOURCES = constants.cc \
			    jid.cc \
			    saslmechanism.cc \
			    xmppclient.cc \
			    xmppengineimpl.cc \
			    xmppengineimpl_iq.cc \
			    xmpplogintask.cc \
			    xmppstanzaparser.cc \
			    xmpptask.cc \
			    ratelimitmanager.cc 



AM_CPPFLAGS = -DPOSIX

#noinst_LTLIBRARIES = libcricketxmpp.la
