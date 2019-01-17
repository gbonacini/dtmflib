lib_LTLIBRARIES = libdtmf.la

libdtmf_la_SOURCES   = dtmf.cpp
libdtmf_la_LDFLAGS   = -version-info 1:0:0  
libdtmf_la_CPPFLAGS  = -I../include

bin_PROGRAMS         = dtmf
dist_man_MANS        = ../doc/dtmf.1
# dist_bin_SCRIPTS = 

nobase_include_HEADERS  = ../include/dtmf.hpp
dtmf_SOURCES        = dtmfutil.cpp  parseCmdLine.cpp 
dtmf_CPPFLAGS       = -I../include 
dtmf_LDADD          = libdtmf.la

ACLOCAL_AMFLAGS = -I m4


