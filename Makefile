# Define the compiler
CXX = g++

# Define the paths
QUICKFIX_HOME = ~/quickfix
PROJECT_HOME = ~/personal/FIX-Engine

# Define the compiler flags
CXXFLAGS = -std=c++11 -Wall -I$(QUICKFIX_HOME)/include -finline-functions -fexceptions

# Define the linker flags
LDFLAGS = -L$(QUICKFIX_HOME)/lib -lquickfix -lpthread -lxml2 -lz

# Define the source files
SRCS = $(PROJECT_HOME)/src/FIXInitiator.cpp $(PROJECT_HOME)/src/FIXApp.cpp $(PROJECT_HOME)/src/utils.cpp 

# Define the object files
OBJS = $(SRCS:.cpp=.o)

# Define the executable name
TARGET = build/fix-engine

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)
