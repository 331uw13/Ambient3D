FLAGS = -std=c++20 -ggdb -Wall -Wextra -Wno-switch
CXX = g++

TARGET_NAME = ambient3d-server

SRC = $(shell find ./src -type f -name *.cpp)
SHARED = ../shared/
OBJS = $(SRC:.cpp=.o)

all: $(TARGET_NAME)


%.o: %.cpp
	@$(CXX) $(FLAGS) -I$(SHARED) -c $< -o $@ && (echo -e "\033[32m[Compiled]\033[0m $<") || (echo -e "\033[31m[Failed]\033[0m $<"; exit 1) 

$(TARGET_NAME): $(OBJS)
	@echo -e "\033[90mLinking...\033[0m"
	@$(CXX) $(OBJS) -o $@ $(FLAGS) && (echo -e "\033[36mDone.\033[0m"; ls -lh $(TARGET_NAME))

clean:
	rm $(OBJS) $(TARGET_NAME)

.PHONY: all clean

