#/***************************************************************************************/
#/*                                                                                     */
#/*  Copyright(c)   .,Ltd                                                               */
#/*                                                                                     */
#/***************************************************************************************/
#/*  file name                                                                          */
#/*             Makefile                                                                */
#/*                                                                                     */
#/*  version                                                                            */
#/*             1.0                                                                     */
#/*                                                                                     */
#/*  description                                                                        */
#/*                                                                                     */
#/*                                                                                     */
#/*  component                                                                          */
#/*                                                                                     */
#/*  author                                                                             */
#/*             bjwf       bjwf2000@gmail.com                                           */
#/*                                                                                     */
#/*  histroy                                                                            */
#/*             2008-11-16     initial draft                                            */
#/***************************************************************************************/

PRGNAME = ccniserver 
MAKE    = make --no-print-directory -s 
CCNI_DIR        = ./ccnimsg
FRAMEWORK_DIR   = ./framework
UTILS_DIR       = ./utils
DBI_DIR         = ./dbi
SCLIENT_DIR     = ./sclient
TOOL_DIR        = ./tool
LIBCCNICLIENT_DIR = ./libccni

export LIBCCNICLIENT = libccni
export CCNI_LIB      = libccnimsg
export UTILS_LIB     = libutils
export DBI_LIB       = libdbi
export CCNI_SERVER   = $(PRGNAME)
export CCNI_SCLIENT  = ccni
export CCNI_TOOL     = ccnitool

CURDIR := $(shell pwd)

ifndef ($(DEBUG))
DEBUG = true
endif

ifeq ($(DEBUG), true) 
    ifndef ($(LOG_LEVEL))
        LOG_LEVEL = 3 
    endif
else
    ifndef ($(LOG_LEVEL))
        LOG_LEVEL = 2 
    endif
endif

ifeq ($(DEBUG), true) 
CFLAG  = -Wall -pipe -ggdb -D_DEBUG -DLOG_LEVEL=$(LOG_LEVEL)
else
CFLAG  = -Wall -pipe -O3 -DLOG_LEVEL=$(LOG_LEVEL)
endif

export CFLAG

export  CC = g++
export  LD = g++
export  AR = ar
export  BIN_DIR = $(CURDIR)/../bin/

export  INC  = -I/usr/include/libxml2 -I$(CURDIR)/inc
export  LIBS = -L/usr/lib -lpthread -lxml2 -lssl 
#-lcrypt -lssl3

export DEBUG

.PHONY : all
all : DISPTITLE $(CCNI_SERVER) mkccni mklibccni_client

$(CCNI_SERVER):mklibutils mklibccni mkdbi mkccniserver mkccnitool
mkccniserver:
	@echo start "make" $(CCNI_SERVER) ...
	$(MAKE) -C $(FRAMEWORK_DIR)
	@echo "make" $(CCNI_SERVER) complete
	@echo
mklibccni:
	@echo start "make" $(CCNI_LIB) ...
	$(MAKE) -C $(CCNI_DIR)
	@echo "make" $(CCNI_LIB) complete
	@echo

mklibutils:
	@echo start "make" $(UTILS_LIB)...
	$(MAKE) -C $(UTILS_DIR)
	@echo "make" $(UTILS_LIB) complete
	@echo

mkdbi:
	@echo start "make" $(DBI_LIB)...
	$(MAKE) -C $(DBI_DIR)
	@echo "make" $(DBI_LIB) complete
	@echo
mkccni:
	@echo start "make" $(CCNI_SCLIENT)...
	$(MAKE) -C $(SCLIENT_DIR)
	@echo "make" $(CCNI_SCLIENT) complete
	@echo
mkccnitool:
	@echo start "make" $(CCNI_TOOL)...
	$(MAKE) -C $(TOOL_DIR)
	@echo "make" $(CCNI_TOOL) complete
	@echo
mklibccni_client:
	@echo start "make" $(LIBCCNICLIENT)...
	$(MAKE) -C $(LIBCCNICLIENT_DIR)
	@echo "make" $(LIBCCNICLIENT) complete
	@echo
DISPTITLE:
	@echo "***********************************************"
	@echo DEBUG   = $(DEBUG)
	@echo CC      = $(CC)
	@echo CFLAG   = $(CFLAG)
	@echo MAKE    = $(MAKE)
	@echo "***********************************************"

.PHONY : clean
clean:
	@echo "make clean..."
	- $(MAKE) -C $(UTILS_DIR)       clean
	- $(MAKE) -C $(CCNI_DIR)        clean
	- $(MAKE) -C $(DBI_DIR)         clean
	- $(MAKE) -C $(FRAMEWORK_DIR)   clean
	- $(MAKE) -C $(SCLIENT_DIR)   clean
	- $(MAKE) -C $(TOOL_DIR)   clean
	- $(MAKE) -C $(LIBCCNICLIENT_DIR) clean




