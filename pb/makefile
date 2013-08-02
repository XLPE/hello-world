XX = g++
CPPFLAGS = -Wall
LIBRARY = -lprotobuf

objects = $(wildcard *.o)
bins = write read reflection

all: $(bins)

$(objects): %.o: %.cpp
	$(XX) $(CPPFLAGS) $< -o $@


write: write.o addressbook.pb.o
	$(XX) $(CPPFLAGS) $(LIBRARY) $^ -o $@

read: read.o addressbook.pb.o
	$(XX) $(CPPFLAGS) $(LIBRARY) $^ -o $@
reflection: reflection.o addressbook.pb.o
	$(XX) $(CPPFLAGS) $(LIBRARY) $^ -o $@

.PHONY: clean

clean:
	-rm -rf *.o
	-rm $(bins)
