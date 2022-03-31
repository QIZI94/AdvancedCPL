CC= g++
CFLAGS= 
OPTIMIZATION= -O2
STDCPP= -std=c++17
CINCLUDE = -Iinclude/ -Ilib/
LIB = -ludev
CPP=#= main.cpp src/common_components/FloorCalibration.cpp src/common_components/SimpleOpenGlTrackerView.cpp src/CameraTracking/CameraObserver.cpp src/CameraTracking/LightBallTracker.cpp
#tools
CPP += src/tools/property.cpp

# modules
CPP += src/modules/shared/moduleinitialization.cpp
CPP += src/modules/usbmod.cpp
CPP += src/modules/settingsmod.cpp

CPP += src/ui/cli/cliui.cpp
OUT= bin/acpl

all: $(CPP)
	mkdir -p bin
	$(CC) $(CFLAGS) $(OPTIMIZATION) $(STDCPP) -o $(OUT)  $(CINCLUDE) $(CPP) $(LIB)

run: $(CPP)
	mkdir -p bin
	$(CC) $(CFLAGS) $(OPTIMIZATION) $(STDCPP) -o $(OUT) $(CINCLUDE) $(CPP) $(LIB)
	./$(OUT)

debug: $(CPP)
	mkdir -p bin
	$(CC) $(CFLAGS) -g $(STDCPP) -o $(OUT)_debug $(CINCLUDE) $(CPP) $(LIB)

clean:
	if [ -e $(OUT) ]; then rm $(OUT); fi
	if [ -e $(OUT)_debug ]; then rm $(OUT)_debug; fi