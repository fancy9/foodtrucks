CC=g++
CFLAGS=-c -Wall -Wcpp
LDFLAGS=-lwthttp -lwt -lboost_signals -lpthread -lmongoclient -lboost_thread -lboost_system -lboost_regex
SOURCES=main.cpp UccDB.cpp UccDBQuery.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=foodtrucks

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm $(OBJECTS) $(EXECUTABLE)