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
OBJS     := $(SRCS:.cpp=.o)
DEPENDS  := $(SRCS:.cpp=.d)


TARGET = $(BIN_DIR)$(CCNI_LIB).a 

.PHONY : all
all : $(TARGET)

$(TARGET): $(OBJS) $(DEPENDS)
	$(AR) -r $(TARGET) $(OBJS)  


%.o:%.cpp
	echo $(CC) $(CFLAG) -o $@ -c $<
	$(CC) $(CFLAG) -o $@ -c $<

%.d: %.cpp
	echo "make dependency" $@
	set -e; rm -f $@; \
        $(CC) -M $(CFLAG) $< > $@.$$$$; \
        sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
        rm -f $@.$$$$

-include ${DEPENDS}


.PHONY : clean	
clean:
	-rm -f $(OBJS) $(TARGET) $(DEPENDS)	
	

	
