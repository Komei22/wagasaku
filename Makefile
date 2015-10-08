#
CXX = g++

CXXFLAGS = -g -Wall -I/usr/local/include -O3
LFLAGS = -L/usr/local/lib -lboost_program_options-mt

EXENAME = main
ALL_CXX_SRC_FILES = main.cpp convertMachine.cpp fileio.cpp
ALL_CXX_OBJ_FILES = $(ALL_CXX_SRC_FILES:.cpp=.o)

all: $(EXENAME)

clean:
	rm -f $(ALL_CXX_OBJ_FILES)
	rm -f $(EXENAME)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXENAME): $(ALL_CXX_OBJ_FILES) $(ALL_C_OBJ_FILES) 
	$(CXX) $(CXXFLAGS) -o $@ $(ALL_CXX_OBJ_FILES) $(ALL_C_OBJ_FILES) $(LFLAGS)

main.o: fileio.hpp convertMachine.hpp student.hpp teacher.hpp
counvertMachine.o: fileio.hpp convertMachine.hpp student.hpp teacher.hpp
fileio.o: fileio.hpp convertMachine.hpp student.hpp teacher.hpp
