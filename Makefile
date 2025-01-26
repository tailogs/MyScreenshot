CC = gcc
CFLAGS = -Wall -Wextra -I"C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/um"
LDFLAGS = -L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/x64" -lgdiplus -lgdi32 -luser32 -mwindows
RES = windres
RES_FLAGS = -I.

TARGET = MyScreenshot.exe
OBJ = main.o resource.o
SRC = main.c resource.rc

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

resource.o: $(SRC)
	$(RES) $(RES_FLAGS) resource.rc resource.o

clean:
	del *.o
	del *.exe
