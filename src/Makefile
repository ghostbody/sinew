main:Main.c Global Catalog Json Serializer Find
	gcc Main.c Catalog.o Json.o Serializer.o Find.o -o sinew -g
Global:Global.h
	gcc Global.h -c -g
Catalog:Catalog.h Catalog.c
	gcc Catalog.h Catalog.c -c -g
Json:Json.h Json.c
	gcc Json.h Json.c -c -g
Serializer:Serializer.h Serializer.c
	gcc Serializer.h Serializer.c -c -g
Find:Find.h Find.c
	gcc Find.h Find.c -c -g
test:
	make
	make clean
	echo "done"
insert:
	./sinew -i testdata_extra.txt
checkout:
	./sinew -cc
find:
	./sinew -f thousandth = 93
clean:
	rm -f sinew *.c~ *.h~ *.o *.h.gch
	rm -f *.*~ *~ *.bin .*.bin.* CATALOG.txt test_JsonSerializer.txt

