CFLAGS = -g
OBJECTS = main.o graphic.o
OUTPUT = program

all: $(OBJECTS)
	cc $(OBJECTS) -o $(OUTPUT)

main.o : main.c
	cc -c main.c $(CFLAGS)

graphic.o : graphic.c
	cc -c graphic.c $(CFLAGS)

clean:
	rm *.o $(OUTPUT)

# Implicit Rules
# 1)	It is not necessary to spell out the recipes for compiling the individual C/CPP source files.
# 		It has an implicit rule for updating a ‘.o’ file from a correspondingly named ‘.c’/'.cpp' file
# 		using a ‘cc -c’/'g++ -c' command.
#
# 2) 	Recipes in built-in implicit rules make liberal use of certain predefined variables.
# 		Example: By redefining ‘CPPFLAGS’ to be ‘-g’, you could pass the ‘-g’ option to each compilation.