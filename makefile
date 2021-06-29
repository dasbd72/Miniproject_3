ifeq ($(OS),Windows_NT)
OSTYPE = windows
else
OSTYPE = linux
endif

PLAYER1 = player
# PLAYER2 = player9-15
PLAYER2 = baseline/$(OSTYPE)/baseline5

CXX			= g++
CXXFLAGS	= --std=c++14 -O2
SOURCES		= $(wildcard *.cpp)
ifeq ($(OS),Windows_NT)
EXE			= $(SOURCES:%.cpp=%.exe)
else
EXE			= $(SOURCES:%.cpp=%)
endif
OTHER		= action state gamelog.txt

.PHONY: all clean

all: $(EXE)

ifeq ($(OS),Windows_NT)
$(EXE): %.exe : %.cpp
	$(CXX) -Wall -Wextra $(CXXFLAGS) -o $@ $<
else
$(EXE): % : %.cpp
	$(CXX) -Wall -Wextra $(CXXFLAGS) -o $@ $<
endif

clean:
ifeq ($(OS),Windows_NT)
	del /f $(EXE) $(OTHER)
else
	rm -f $(EXE) $(OTHER)
endif

runO:
	make all
ifeq ($(OS),Windows_NT)
	./main.exe ./$(PLAYER1).exe ./$(PLAYER2).exe
else
	./main ./$(PLAYER1) ./$(PLAYER2)
endif
	make clean

runX:
	make all
ifeq ($(OS),Windows_NT)
	./main.exe ./$(PLAYER2).exe ./$(PLAYER1).exe
else
	./main ./$(PLAYER2) ./$(PLAYER1)
endif
	make clean