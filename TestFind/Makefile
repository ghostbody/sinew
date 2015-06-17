main:*.c *h
	gcc Global.h Json.h Json.c Find.h Find.c Catalog.c Catalog.h Serializer.h Serializer.c main.c -o test -g
test:
	make
	make clean
clean:
	rm -f sinew *.c~ *.h~ *.o *.h.gch
	rm -f *.*~ *~ 

