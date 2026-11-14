
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -fno-exceptions

SOURCES = src/grp_102_main.cpp \
          src/grp_102_erp.cpp \
          src/grp_102_course.cpp \
          src/grp_102_student.cpp \
          src/grp_102_registration.cpp \
          src/grp_102_commands.cpp \
          src/grp_102_gpa.cpp \
          src/grp_102_persistence.cpp

TARGET = acaderp

all: $(TARGET)

$(TARGET): $(SOURCES) src/grp_102_erp.h
	$(CXX) $(CXXFLAGS) -Isrc $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o src/*.o

.PHONY: all clean
