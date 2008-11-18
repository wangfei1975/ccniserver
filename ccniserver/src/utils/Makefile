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
#/*             2008-06-01     initial draft                                            */
#/***************************************************************************************/

ifndef DEBUG
DEBUG = true
CC = g++323
LD = g++323
AR = ar
CFLAG  = -Wall -pipe -ggdb -D _DEBUG
endif

ifdef NO_CENTER
CFLAG += -DWN_NO_CENTERSRV
endif

INC      += -I.

CFLAG    += $(INC)

SRCS     := $(wildcard *.cpp)
OBJS      = $(SRCS:.cpp=.o)


TARGET = $(BIN_DIR)$(UTILS_LIB).a 

.PHONY : all
all : $(TARGET)

$(TARGET): $(OBJS)
	$(AR) r $(TARGET) $(OBJS)  


%.o:%.cpp
	echo $(CC) $(CFLAG) -o $@ -c $?
	$(CC) $(CFLAG) -o $@ -c $?

.PHONY : clean	
clean:
	-rm -f $(OBJS) $(TARGET) 	
	

	