# Define variables
OBJECTS = multithread_matrix.o 
CFLAGS = -g
LDFLAGS = -g

# Compile .c files to .o files
%.o: %.c 
	clang $(CFLAGS) -c $< -o $@

# Link object files to executable
demo: $(OBJECTS)
	clang $(LDFLAGS) $(OBJECTS) -o multithread_matrix

# Clean up object files and executable
clean:
	rm -f multithread_matrix $(OBJECTS)
