# Ime datoteke programa
TARGET_EXEC := matrix-calc

# Direktorij u koji će biti pohranjeni rezultati kompajliranja
BUILD_DIR := build
# Direktorij u kojem se nalaze source datoteke
SRC_DIRS := src
# Direktorij u kojem se nalaze header datoteke
INC_DIRS := include

ifdef OS
	RM := rmdir /s /q
	MKDIR := mkdir
	FixPath = $(subst /,\,$(1))
	SRCS := $(shell @echo off & for /f %A in ('forfiles /p $(SRC_DIRS) /s /m *.cpp /c "cmd /c echo @relpath"') do echo $(SRC_DIRS)\%~A)
else
	ifeq ($(shell uname), Linux)
		RM := rm -rf
		MKDIR := mkdir -p
		FixPath = $(1)
		SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c')
	endif
endif

# Kreiraj listu objektnih datoteka
OBJS := $(call FixPath, $(SRCS:%=$(BUILD_DIR)/%.o))

# Kreiraj zastavice za direktorije sa header datotekama
INC_FLAGS := $(addprefix -I,$(call FixPath, $(INC_DIRS)))

# Postavi zastavice općenito
CPPFLAGS := $(INC_FLAGS)
# Postavi kompajlere za C++ i C
CXX := g++
CC := gcc

# Spoji sve objektne datoteke u krajnji program
$(call FixPath, $(BUILD_DIR)/$(TARGET_EXEC)): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Kompajliraj C kod
$(call FixPath, $(BUILD_DIR)/%.c.o): %.c
	$(MKDIR) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Kompajliraj C++ kod
$(call FixPath, $(BUILD_DIR)/%.cpp.o): %.cpp
	$(MKDIR) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(BUILD_DIR)

testpathfix:
	echo $(call FixPath, /etc/bin)