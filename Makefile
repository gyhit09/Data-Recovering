CFLAGS += -Wall -O3


.PHONY :  clean

test : test.o readev.o 
	$(CC) $(CFLAGS) -o $@ $^

clean :
	$(RM) *.o *~ core* test
