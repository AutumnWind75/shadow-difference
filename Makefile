CXX = g++
CXXFLAGS = -std=c++17 -O2

ifeq ($(OS),Windows_NT)
    EXE_SUFFIX = .exe
    RM = cmd /c del /q
else
    EXE_SUFFIX =
    RM = rm -f
endif

TARGET1 = random_shadow$(EXE_SUFFIX)
TARGET2 = shadow_mod$(EXE_SUFFIX)
all: $(TARGET1) $(TARGET2)

$(TARGET1): random_shadow.cpp
	$(CXX) $(CXXFLAGS) -pthread -o $@ $<
$(TARGET2): shadow_mod.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	$(RM) $(TARGET1) $(TARGET2)

.PHONY: all clean