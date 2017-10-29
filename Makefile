CXX   = g++
CXXFLAGS = -Wall -std=c++11
MAIN     = main
_MAIN    = ${MAIN:%=%.cc}
SRCS     = request tui bank
_SRCS    = ${SRCS:%=%.cc}
_OBJS    = ${SRCS:%=%.o}
TRGT     = a.exe
LNKFLGS  = -pthread

all:
	$(CXX) $(CXXFLAGS) -c $(_SRCS)
	$(CXX) $(CXXFLAGS) -o $(TRGT) $(_MAIN) $(_OBJS) $(LNKFLGS)
clean:
	rm $(TRGT) $(_OBJS)
