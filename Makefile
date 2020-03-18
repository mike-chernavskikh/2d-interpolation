NAME            = plot

CC              = gcc
LINK            = gcc

DEBUG           = 
OPTIMAZE_COMMON   = -O3 -ffast-math
OPTIMAZE_SPECIFIC = -lm
OPTIMAZE        = $(OPTIMAZE_COMMON) $(OPTIMAZE_SPECIFIC)

WARNINGS        = -W -Wall
INCLUDE_DIR     = -I. -I/usr/X11R6/include
CFLAGS          = -c $(DEBUG) $(OPTIMAZE) $(WARNINGS) $(INCLUDE_DIR)

LIB_DIR         = -L. -L/usr/X11R6/lib
LIB             = -lX11
LDFLAGS         = $(DEBUG)

OBJS            = plot_x11.o test.o

all:    $(NAME)

$(NAME)         : $(OBJS)
	$(LINK) $(LDFLAGS) $(OBJS) $(LIB_DIR) $(LIB) -lm -o $(NAME)

clean:
	rm -f $(OBJS) $(NAME)

.c.o:
	$(CC) $(CFLAGS) $<

#-------------------------------------------------------------------------------

# ��������� � X11
plot_x11.o      : plot_x11.c plot_x11.h
# ����
test.o          : test.c plot_x11.h

#-------------------------------------------------------------------------------
