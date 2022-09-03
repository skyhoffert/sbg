###################################################################################################
#
# Filename: Makefile
# Description: Makefile for sbg.
#
# Additional Notes:
# ------------------
# None.
#
# Development History:
#  Date         Author        Description of Change
# ------       --------      -----------------------
# 2022-08-26   Sky Hoffert   Initial release.
# 2022-09-02   Sky Hoffert   Switching to creating the shared library for executable.
#
###################################################################################################

CC = gcc
CFLAGS = -Wall -Werror -L.
CLIBS = -lpthread -lcrypto

all: testsbg
	$(info Making all.)

testsbg: testsbg.c sbg.o libsbg.so
	$(info Making testsbg.)
	$(CC) $(CFLAGS) -Wl,-rpath=. -o testsbg testsbg.c -lsbg $(CLIBS)

libsbg.so: sbg.o
	$(CC) -shared -fPIC -o libsbg.so sbg.o

sbg.o: sbg.h sbg.c
	$(info Making sbg.o.)
	$(CC) $(CFLAGS) -fPIC -o sbg.o -c sbg.c $(CLIBS)
