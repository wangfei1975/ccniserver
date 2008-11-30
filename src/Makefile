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
CCNI_DIR        = ./ccni
FRAMEWORK_DIR   = ./framework
UTILS_DIR       = ./utils
DBI_DIR         = ./dbi
SCLIENT_DIR     = ./sclient

export CCNI_LIB      = libccni
export UTILS_LIB     = libutils
export DBI_LIB       = libdbi
export CCNI_SERVER   = $(PRGNAME)
export CCNI_SCLIENT  = ccni

CURDIR := $(shell pwd)

ifndef ($(DEBUG))
DEBUG = true
endif

ifeq ($(DEBUG), true) 
    ifndef ($(LOG_LEVEL))
        LOG_LEVEL = 5
    endif
else
    ifndef ($(LOG_LEVEL))
        LOG_LEVEL = 1
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
all : DISPTITLE $(CCNI_SERVER) mkccni

$(CCNI_SERVER):mklibutils mklibccni mkdbi mkccniserver
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



