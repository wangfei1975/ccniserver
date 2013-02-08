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
CC = g++
LD = g++
AR = ar
CFLAG  = -Wall -pipe -ggdb -D _DEBUG
endif
 

INC      += -I.

CFLAG    += $(INC)

SRCS     := $(wildcard *.cpp)
SRCC     := $(wildcard *.c)
OBJC     := $(SRCC:.c=.o)
OBJS     := $(SRCS:.cpp=.o)
OBJS += $(OBJC)
DEPENDS  := $(OBJS:.o=.d)


TARGET   = $(BIN_DIR)$(LIBCCNICLIENT).a
TARGETSO   = $(BIN_DIR)$(LIBCCNICLIENT).so


.PHONY : all
all : $(TARGET)

$(TARGET): $(OBJS) $(DEPENDS)
	$(AR) -r $(TARGET) $(OBJS)  
#	$(LD) -shared -o $(TARGETSO) $(LD_FLAG) $(OBJS) $(LIBS)

%.o:%.cpp
	echo $(CC) $(CFLAG) -o $@ -c $<
	$(CC) $(CFLAG) -o $@ -c $<

%.o:%.c
	echo $(CC) $(CFLAG) -o $@ -c $<
	$(CC) $(CFLAG) -o $@ -c $<


%.d:%.cpp
#	echo "make dependency" $@
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAG) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.d:%.c
#	echo "make dependency" $@
	@set -e; rm -f $@; \
        $(CC) -M $(CFLAG) $< > $@.$$$$; \
        sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
        rm -f $@.$$$$
        
-include ${DEPENDS}

.PHONY : clean	
clean:
	-rm -f $(OBJS) $(TARGET) $(TARGETSO) $(DEPENDS) 	
	

	
