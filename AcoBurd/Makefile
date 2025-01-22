# Makefile for ACOBURD content
CXX= g++
CXXFLAGS = -Wall -std=c++11

TARGET = ACOBURD

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
       $(CXX) $(CXXFLAGS) $(OBJS) -o $(MAIN) 


%.o: %.cpp
		$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
        rm -f $(OBJS) $(TARGET)
