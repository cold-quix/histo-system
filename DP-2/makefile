#
# =======================================================
#                    Data Producer 2
# =======================================================
# FINAL BINARY DP-2
../common/bin/DP-2 : ./obj/DP-2.o
	cc ./obj/DP-2.o -o ../common/bin/DP-2
#
# =======================================================
#                     Dependencies
# =======================================================
./obj/DP-2.o : ./src/dp2.c ./inc/prototypes.h ../common/inc/prototypes.h ../common/inc/constants.h ../common/inc/semaphores.h
	cc -c ./src/dp2.c -o ./obj/DP-2.o

#
# =======================================================
# Other targets
# =======================================================
clean:
	rm -f ../common/bin/*
	rm -f ./obj/*
	rm -f ./src/dp2.c~
