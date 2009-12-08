#/***************************************************************************************/
#/*                                                                                     */
#/*  Copyright(c)   .,Ltd                                               */
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
#/*             2008-06-01     initial draft                                            */
#/***************************************************************************************/
export MAKE = make  --no-print-directory -s

export PRGNAME=ccniserver

export DATE=$(shell date +%Y%m%d)
PWD:=$(shell pwd)

.PHONY : all
all :  
	$(MAKE) -C ./src 

tar:clean
	cd ..;tar --exclude='.svn' -czvf $(PRGNAME).$(DATE).tar.gz ccniserver;cd -

.PHONY : clean
clean:
	- $(MAKE) -C ./src   clean
	-rm -f ./bin/logs/*




