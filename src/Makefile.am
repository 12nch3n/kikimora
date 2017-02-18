AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS=kikimora
kikimora_LDADD=-ljsoncpp -lxerces-c
kikimora_SOURCES=kikimora.cpp \
				 lib/json/jsoncpp.cpp \
				 lib/docopt/docopt.cpp
AM_CPPFLAGS=-std=c++11 \
			-Ilib
