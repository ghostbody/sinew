main:main.c Global Catalog Json Serializer
	gcc main.c Catalog.o Json.o Serializer.o -o test -g
Global:Global.h
	gcc Global.h -c -g
Catalog:Catalog.h Catalog.c
	gcc Catalog.h Catalog.c -c -g
Json:Json.h Json.c
	gcc Json.h Json.c -c -g
Serializer:Serializer.h Serializer.c
	gcc Serializer.h Serializer.c -c -g
big:
	./test <./data/testdata.txt
small:
	./test <./data/ajson.txt
medium:
	./test <./data/testdata_extra.txt
clean:
	rm -f test *.c~ *.h~ *.o *.h.gch
	rm -f *.*~ *~ *.bin .*.bin.* CATALOG.txt test_JsonSerializer.txt
