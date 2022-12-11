# Ime datoteke programa
TARGET_EXEC := matrix-calc

# Direktorij u koji će biti pohranjeni rezultati kompajliranja
BUILD_DIR := ./build
# Direktorij u kojem se nalaze source datoteke
SRC_DIRS := ./src
# Direktorij u kojem se nalaze header datoteke
INC_DIRS := ./include

# Kreiraj listu source i objektnih datoteka
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# Kreiraj zastavice za direktorije sa header datotekama
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Postavi zastavice općenito
CPPFLAGS := $(INC_FLAGS)
# Postavi kompajlere za C++ i C
CXX := g++
CC := gcc

# Spoji sve objektne datoteke u krajnji program
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Kompajliraj C kod
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Kompajliraj C++ kod
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -r $(BUILD_DIR)
