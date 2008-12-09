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

INC      += -I.

CFLAG    += $(INC)

SRCS     := $(wildcard *.cpp)
OBJS      = $(SRCS:.cpp=.o)


TARGET = $(BIN_DIR)$(CCNI_TOOL) 
LIBS   += $(BIN_DIR)$(CCNI_LIB).a $(BIN_DIR)$(UTILS_LIB).a $(BIN_DIR)$(DBI_LIB).a
#LIBS += $(BIN_DIR)$(UTILS_LIB).a

.PHONY : all
all : $(TARGET)

$(TARGET): $(OBJS)
	echo $(LD) -o $(TARGET) $(LD_FLAG) $(OBJS) $(LIBS) 
	$(LD) -o $(TARGET) $(LD_FLAG) $(OBJS) $(LIBS)


%.o:%.cpp
	echo $(CC) $(CFLAG) -o $@ -c $?
	$(CC) $(CFLAG) -o $@ -c $?

.PHONY : clean	
clean:
	-rm -f $(OBJS) $(TARGET) 	
	

	
