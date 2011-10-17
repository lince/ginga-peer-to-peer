# set the include path found by configure
INCLUDES =	-I/usr/include/ \
			-I/usr/local/include/ \
			-I/usr/local/include/directfb/ \
			-I/usr/include/xercesc/ \
			$(all_includes)
        
LIBS =

# the library search path.
localLibdir = /usr/local/lib/ginga

# Quando o componente P2P estiver sendo acessado através do ComponentManager,
# acho que não vai mais precisar disso.
AM_CXXFLAGS = -DPOSIX

libp2ptestapp_so_LDFLAGS = -Wl,-soname,libp2ptestapp.so -shared \
	-L/usr/local/lib \
	-L/usr/local/lib/ginga \
	-L/usr/local/lib/ginga/iocontents \
	-L/usr/local/lib/libjingle \
	-L/usr/lib
	
localLib_PROGRAMS = libp2ptestapp.so

libp2ptestapp_so_SOURCES = P2PTest.cpp GraphicInterface.cpp GraphicList.cpp

libp2ptestapp_so_LDADD = -lgingaccp2p \
	-lcricketbase -lcricketsessionfileshare \
	-lcricketp2pbase -lcricketp2pclient -lcricketexampleslogin \
	-lcricketsessiontunnel -lcricketxmllite -lcricketxmpp \
	-lcrickettest -lpthread -lgingaccsystemthread -llinceutil \
	-lgingaccsystemiodfb -lgingaccsysctsdfbf \
	-lgingaccsysctsimgdfb -lxerces-c