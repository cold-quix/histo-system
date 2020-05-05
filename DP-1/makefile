#
# =======================================================
#                    Data Producer 1
# =======================================================
# FINAL BINARY DP-1
../common/bin/DP-1 : ./obj/DP-1.o
	cc ./obj/DP-1.o -o ../common/bin/DP-1
#
# =======================================================
#                     Dependencies
# =======================================================
./obj/DP-1.o : ./src/dp1.c ./inc/prototypes.h ../common/inc/prototypes.h ../common/inc/constants.h ../common/inc/semaphores.h
	cc -c ./src/dp1.c -o ./obj/DP-1.o

#
# =======================================================
# Other targets
# =======================================================
clean:
	rm -f ../common/bin/*
	rm -f ./obj/*
	rm -f ./src/dp1.c~
