CXX      = g++
CXXFLAGS = -std=c++11 -Wall -g

INCLUDES = -I include -I src/cache -I include/vm -I src/vm

TARGET   = final_sim

SRCS     = main.cpp src/cache/cache.cpp

OBJS     = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS)
	@echo "Build Complete! Run ./$(TARGET)"

%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "Cleaning up..."
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean