cc = g++
cppflags=-c
prom = main main1 main2
deps = ALU.h Command.h Database.h DatabaseMap.h Row.h Table.h
obj = main.o ALU.o Command.o Database.o DatabaseMap.o Row.o Table.o
all: $(prom)

INTERNET: cppflags=-c -DINTERNET
INTERNET: $(prom)

main: $(obj) main.o
	$(cc) -o main $(obj)
main1: $(obj) main.o
	$(cc) -o main1 $(obj)
main2: $(obj) main.o
	$(cc) -o main2 $(obj)

%.o: %.cpp $(deps)
	$(cc) $(cppflags) $< -o $@

clean:
	-rm *.o $(prom)