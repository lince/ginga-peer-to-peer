METASOURCES = AUTO
LIBS =

libcricketxmlliteIncludedir = /usr/local/include/libjingle/talk/xmllite

libcricketxmlliteInclude_HEADERS =  	qname.h \
			        	xmlbuilder.h \
			        	xmlconstants.h \
			        	xmlelement.h \
			        	xmlnsstack.h \
			        	xmlparser.h \
			        	xmlprinter.h

#set the include path found by configure
INCLUDES = -I/usr/local/include/ -I/usr/local/include/ginga \
	$(all_includes)


libcricketxmllitedir = /usr/local/lib/libjingle

libcricketxmllite_so_LDFLAGS = -shared \
    -Wl,-soname,libcricketxmllite.so \
    -L/usr/local/lib -L/usr/local/lib/ginga -L../thread -L./

libcricketxmllite_PROGRAMS = libcricketxmllite.so

libcricketxmllite_so_SOURCES = qname.cc \
			       xmlbuilder.cc \
			       xmlconstants.cc \
 			       xmlelement.cc \
			       xmlnsstack.cc \
			       xmlparser.cc \
			       xmlprinter.cc

AM_CPPFLAGS = -DPOSIX

#noinst_LTLIBRARIES = libcricketxmllite.la
