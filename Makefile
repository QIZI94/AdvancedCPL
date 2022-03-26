CC= g++
CFLAGS= 
OPTIMIZATION= -O2
STDCPP= -std=c++17
CINCLUDE = -Iinclude/ -Ilib/
#LIB = -lopencv_core -lopencv_highgui -lopencv_videoio -lopencv_imgproc -lopencv_objdetect -Bsymbolic -l SDL2 -lGL -lGLU
CPP=#= main.cpp src/common_components/FloorCalibration.cpp src/common_components/SimpleOpenGlTrackerView.cpp src/CameraTracking/CameraObserver.cpp src/CameraTracking/LightBallTracker.cpp
#tools
CPP += src/tools/property.cpp

# modules
CPP += src/modules/shared/moduleinitialization.cpp
CPP += src/modules/settingsmod.cpp

CPP += src/ui/cli/cliui.cpp
OUT= bin/ovr_test

all: $(CPP)
	mkdir -p bin
	$(CC) $(CFLAGS) $(OPTIMIZATION) $(STDCPP) -o $(OUT) $(LIB) $(CINCLUDE) $(CPP) 

run: $(CPP)
	mkdir -p bin
	$(CC) $(CFLAGS) $(OPTIMIZATION) $(STDCPP) -o $(OUT) $(LIB) $(CINCLUDE) $(CPP) 
	./$(OUT)

debug: $(CPP)
	mkdir -p bin
	$(CC) $(CFLAGS) -g $(STDCPP) -o $(OUT)_debug $(LIB) $(CINCLUDE) $(CPP)

clean:
	if [ -e $(OUT) ]; then rm $(OUT); fi
	if [ -e $(OUT)_debug ]; then rm $(OUT)_debug; fi