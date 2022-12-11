# ----------------- Odabir imena direktorija -----------------

# Ime datoteke programa
TARGET_EXEC := matrix-calc
# Direktorij u koji će biti pohranjeni rezultati kompajliranja
BUILD_DIR := build
# Direktorij u kojem se nalaze source datoteke
SRC_DIR := src
# Direktorij u kojem se nalaze header datoteke
INC_DIRS := include


# ------------------- Ako je OS MS Windows -------------------

ifdef OS
# Postavi naredbu za brisanje direktorija
RM := rmdir /s /q
# Kreiraj funkciju koja stvara naredbu za kreiranje direktorija ako ne postoji
MKDIR = if not exist $1 mkdir $1
# Postavi funkciju FixPath da promjeni slash u backslash
FixPath = $(subst /,\,$(1))
# Kreiraj funkciju koja mijenja backslash u slash kako bi prilagodila podatke dobivene iz windowsa
ToLinuxSlashes = $(subst \,/,$(1))
# Postavi shell na CMD
SHELL=C:\Windows\system32\cmd.exe
# Pronađi sve source datoteke u SRC_DIR direktoriju
SRCS := $(call ToLinuxSlashes, $(shell for /f %%A in ('forfiles /p $(SRC_DIR) /s /m *.cpp /c "cmd /c echo @relpath"') do echo $(SRC_DIR)\%%~A))


# --------------------- Ako je OS Linux ----------------------

else
ifeq ($(shell uname), Linux)
# Postavi naredbu za brisanje direktorija
RM := rm -rf
# Kreiraj funkciju koja stvara naredbu za kreiranje direktorija ako ne postoji
MKDIR = mkdir -p $1
# S obzirom da linux već koristi slash nije potrebna promjena
FixPath = $(1)
# Pronađi sve source datoteke u SRC_DIR direktoriju
SRCS := $(shell find $(SRC_DIR) -name '*.cpp' -or -name '*.c')
endif
endif


# ---------------- Postavi još neke varijable ----------------

# Kreiraj listu objektnih datoteka
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# Kreiraj zastavice za direktorije sa header datotekama
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Postavi zastavice općenito
CPPFLAGS := $(INC_FLAGS)
# Postavi kompajlere za C++ i C
CXX := g++
CC := gcc


# --------------------- Kompajliraj kod ----------------------

# Spoji sve objektne datoteke u krajnji program
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Kompajliraj C kod
$(BUILD_DIR)/%.c.o: %.c
	$(call FixPath, $(call MKDIR, $(dir $@)))
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Kompajliraj C++ kod
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(call FixPath, $(call MKDIR, $(dir $@)))
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


# ----------- Brisanje svih kompajliranih datoteka -----------

clean:
	$(RM) $(BUILD_DIR)