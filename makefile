CPPFLAGS = -g
OBJECTS = main.o
OUTPUT = program

all: $(OBJECTS)
	cc $(OBJECTS) -o $(OUTPUT)

main.o : stb_image_write.h graphic.c

clean:
	rm *.o $(OUTPUT)

# Implicit Rules
# 1)	It is not necessary to spell out the recipes for compiling the individual C/CPP source files.
# 		It has an implicit rule for updating a ‘.o’ file from a correspondingly named ‘.c’/'.cpp' file
# 		using a ‘cc -c’/'g++ -c' command.
#
# 2) 	Recipes in built-in implicit rules make liberal use of certain predefined variables.
# 		Example: By redefining ‘CPPFLAGS’ to be ‘-g’, you could pass the ‘-g’ option to each compilation.