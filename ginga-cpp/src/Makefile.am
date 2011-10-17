
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/libjingle/ \
$(all_includes)

LIBS =

AM_CXXFLAGS = `pkg-config avahi-client --cflags --libs` -DPOSIX

gingadir = /usr/local/sbin
ginga_PROGRAMS = ginga
ginga_SOURCES = main.cpp
ginga_LDADD = -ltelemidiautil $(COMPONENTS_LIBS)
