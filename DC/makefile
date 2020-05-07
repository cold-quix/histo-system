#
# =======================================================
#                     Data Consumer
# =======================================================
# FINAL BINARY DC
../common/bin/DC : ./obj/DC.o
	cc ./obj/DC.o -o ../common/bin/DC
#
# =======================================================
#                     Dependencies
# =======================================================
./obj/DC.o : ./src/consumer.c ./inc/prototypes.h ../common/inc/prototypes.h ../common/inc/constants.h ../common/inc/semaphores.h
	cc -c ./src/consumer.c -o ./obj/DC.o

#
# =======================================================
# Other targets
# =======================================================
clean:
	rm -f ../common/bin/*
	rm -f ./obj/*
	rm -f ./src/consumer.c~
