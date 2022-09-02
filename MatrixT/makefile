  CC = gcc 
  CFLAGS  = -g -Wall
  RM = rm -f
  TARGET = matrixT
  FLAGS = -pthread
 
  all: $(TARGET) run
  
  $(TARGET): $(TARGET).c
	$(CC) $(FLAGS) $(CFLAGS) -o $(TARGET) $(TARGET).c -w
 
  clean:
	@$(RM) $(TARGET)
		
  run:
	@./$(TARGET)
	
  	
