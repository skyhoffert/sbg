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
#
###################################################################################################

CC = gcc
CFLAGS = -Wall -Werror 
CLIBS = -lpthread -lcrypto

all: testsbg
	$(info Making all.)

testsbg: testsbg.c sbg.o
	$(info Making testsbg.)
	$(CC) $(CFLAGS) -o testsbg testsbg.c sbg.o $(CLIBS)

sbg.o: sbg.h sbg.c
	$(info Making sbg.o.)
	$(CC) $(CFLAGS) -o sbg.o -c sbg.c $(CLIBS)
