CXX = g++
CXXFLAGS = -I include/ -O3
CDBFLAGS = -g

BIN = RoboCar.out

SOURCE = $(wildcard src/*.cpp) \
		 $(wildcard src/PinsLib/*.cpp) \
		 $(wildcard src/RoboCar/*.cpp)

INCLUDE = $(wildcard include/*.h) \
          $(wildcard include/PinsLib/*.h) \
          $(wildcard include/RoboCar/*.h)

OBJSDIR = objs
OBJS = $(patsubst src/%, $(OBJSDIR)/%, $(patsubst %.cpp, %.o, $(SOURCE)))


all: $(BIN)

#debug: $(OBJSDIR) $(OBJS)
#    $(CXX) $(CXXFLAGS) $(CDBFLAGS) -o $@ $(OBJS)

$(BIN): $(OBJSDIR) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(OBJSDIR):
	mkdir -p $@ && \
    mkdir -p $@/PinsLib && \
    mkdir -p $@/RoboCar


$(OBJSDIR)/%.o: src/%.cpp $(INCLUDE)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: run clean

run:
	./$(BIN)

clean:
	rm -rf $(OBJSDIR) && rm $(BIN)