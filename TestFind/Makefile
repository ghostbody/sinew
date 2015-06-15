main:*.c *h
	gcc Global.h Find.h Find.c main.c -o test -g
test:
	make
	make clean
clean:
	rm -f sinew *.c~ *.h~ *.o *.h.gch
	rm -f *.*~ *~ *.bin .*.bin.* CATALOG.txt test_JsonSerializer.txt

