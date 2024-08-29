# # Makefile

# # Compiler
# CXX = g++

# # Compiler flags
# CXXFLAGS = -Wall -std=c++17

# # Source files
# SOURCES = Main.cpp SharedTransaction.cpp User.cpp Transaction.cpp SharedTransaction.cpp Logger.cpp SavingPlan.cpp Date.cpp Participant.cpp

# # Object files (replace .cpp with .o)
# OBJECTS = $(SOURCES:.cpp=.o)

# # Executable name
# EXECUTABLE = Wallet

# # Default rule
# all: $(EXECUTABLE)

# # Link object files to create the executable
# $(EXECUTABLE): $(OBJECTS)
# 	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXECUTABLE)

# # Rule to compile .cpp files into .o files
# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# # Clean up generated files
# clean:
# 	rm -f $(OBJECTS) $(EXECUTABLE)
