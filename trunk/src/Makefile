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

CCNI_DIR        = ./ccni
FRAMEWORK_DIR   = ./framework
UTILS_DIR       = ./utils
DBI_DIR         = ./dbi

export CCNI_LIB      = libccni
export UTILS_LIB     = libutils
export DBI_LIB       = libdbi
export CCNI_SERVER   = $(PRGNAME)

CURDIR := $(shell pwd)

ifndef ($(DEBUG))
DEBUG = true
endif


ifeq ($(DEBUG), true) 
CFLAG  = -Wall -pipe -ggdb -D_DEBUG -DLOG_LEVEL=5
else
CFLAG  = -Wall -pipe -O3 -DLOG_LEVEL=1
endif

export CFLAG

export  MAKE = make  --no-print-directory -s
export  CC = g++
export  LD = g++
export  AR = ar
export  BIN_DIR = $(CURDIR)/../bin/

export  INC = -I/usr/include/libxml2 -I$(CURDIR)/inc
export  LIBS = -lpthread -lxml2 -lssl 
#-lcrypt -lssl3

export DEBUG

.PHONY : all
all : DISPTITLE $(CCNI_SERVER)

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



