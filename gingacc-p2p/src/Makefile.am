# set the include path found by configure
INCLUDES =	-I/usr/include/ \
			-I/usr/local/include/ \
			$(all_includes)

LIBS =

libgingaccp2pdir = /usr/local/lib/ginga

AM_CXXFLAGS = -DPOSIX

libgingaccp2p_so_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga -L/usr/local/lib/libjingle \
	-Wl,-soname,libgingaccp2p.so -shared

libgingaccp2p_PROGRAMS = libgingaccp2p.so

libgingaccp2p_so_SOURCES = P2PManager.cpp FileShareClient.cpp ChatTask.cpp

libgingaccp2p_so_LDADD = -lcricketbase -lcricketsessionfileshare \
	-lcricketp2pbase -lcricketp2pclient -lcricketexampleslogin \
	-lcricketsessiontunnel -lcricketxmllite -lcricketxmpp \
	-lcrickettest -llinceutil $(COMPONENTS_LIBS)
