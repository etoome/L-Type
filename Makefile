CXXFLAGS=-std=c++17 -masm=intel -fconcepts -pthread -fopenacc -fopenmp -ggdb3
CXXDFLAGS=$(CXXFLAGS) -Wpedantic -Wall -Wextra -Wconversion -Winline -Wsign-conversion -Weffc++ -Wstrict-null-sentinel -Wold-style-cast -Wnoexcept -Wctor-dtor-privacy -Woverloaded-virtual -Wsign-promo -Wzero-as-null-pointer-constant -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override

SHARED_SRC=$(wildcard src/*.cpp)
SHARED_OBJ=$(SHARED_SRC:src/%.cpp=obj/%.o)

ASSETS_ID=include/assetsID.hpp
ASSETS_CLI=src/client/cli/Assets.cpp
ASSETS_CLI_SRC=$(wildcard src/client/cli/assets/*.cpp)
ASSETS_CLI_OBJ=$(ASSETS_CLI_SRC:src/client/cli/assets/%.cpp=obj/client/cli/assets/%.o) $(ASSETS_CLI:src/%.cpp=obj/%.o)
ASSETS_GUI=src/client/gui/Assets.cpp
ASSETS_GUI_SRC=$(wildcard src/client/gui/assets/*.cpp)
ASSETS_GUI_OBJ=$(ASSETS_GUI_SRC:src/client/gui/assets/%.cpp=obj/client/gui/assets/%.o) $(ASSETS_GUI:src/%.cpp=obj/%.o)

SERVER_BIN=bin/server
SERVER_MAIN=src/server/main.cpp
SERVER_SRC=$(filter-out $(SERVER_MAIN),$(wildcard src/server/*.cpp)) $(wildcard src/server/game/*.cpp) $(wildcard src/server/sandbox/*.cpp)
SERVER_OBJ=$(SERVER_SRC:src/server/%.cpp=obj/server/%.o)

CLIENT_SRC=$(wildcard src/client/*.cpp)
CLIENT_OBJ=$(CLIENT_SRC:src/client/%.cpp=obj/client/%.o)

CLI_MAIN=src/client/cli/main.cpp
CLI_SRC=$(filter-out $(CLI_MAIN), $(wildcard src/client/cli/*.cpp))
CLI_OBJ=$(CLI_SRC:src/client/cli/%.cpp=obj/client/cli/%.o)
GUI_MAIN=src/client/gui/main.cpp
GUI_SRC=$(filter-out $(GUI_MAIN), $(wildcard src/client/gui/*.cpp))
GUI_OBJ=$(GUI_SRC:src/client/gui/%.cpp=obj/client/gui/%.o)

CLI_BIN=bin/client-cli
GUI_BIN=bin/client-gui

# Pre-build
$(shell mkdir -p lib bin obj/server/game obj/server/sandbox obj/client/cli/assets obj/client/gui/assets)
$(shell ./buildAssets.py)

all: $(SERVER_BIN) $(CLI_BIN) $(GUI_BIN)

-include obj/*.d
-include obj/server/*.d
-include obj/server/game/*.d
-include obj/server/sandbox/*.d
-include obj/client/*.d
-include obj/client/cli/*.d
-include obj/client/cli/assets/*.d
-include obj/client/gui/*.d
-include obj/client/gui/assets/*.d

# Pre-build
static/built:
	@./buildDatabase.py
	@./addDbLevels.py static/levels/level1.csv
	@./addDbLevels.py static/levels/level2.csv
	@./addDbLevels.py static/levels/level3.csv
	@touch static/built

# =============== OBJECTS ============== #
obj/%.o: src/%.cpp
	@g++ -MMD $(CXXFLAGS) -Iinclude -c $< -o $@
obj/client/cli/%.o: src/client/cli/%.cpp
	@g++ -MMD $(CXXFLAGS) -Iinclude -c $< -o $@
obj/client/gui/%.o: src/client/gui/%.cpp
	@g++ -MMD $(CXXFLAGS) -Iinclude -c $< -o $@ -DGUI
# ====================================== #

# ============= SERVER API ============= #
obj/server/CommunicationAPI.o: src/server/CommunicationAPI.cpp include/server/CommunicationAPI.hpp
	@g++ -fPIC -MMD $(CXXFLAGS) -Iinclude -c $< -o $@
obj/server/MessageExchanger.o: src/server/MessageExchanger.cpp include/server/MessageExchanger.hpp
	@g++ -fPIC -MMD $(CXXFLAGS) -Iinclude -c $< -o $@
lib/libCommunicationAPI.a: obj/server/CommunicationAPI.o obj/server/MessageExchanger.o
	@ar rs $@ $^ 2> /dev/null
# ====================================== #

# =============== SERVER =============== #
$(SERVER_BIN): $(SERVER_MAIN) $(SERVER_OBJ) $(SHARED_OBJ)
	@make static/built &> /dev/null
	@g++ $(CXXFLAGS) -Iinclude $^ -o $@ -lsqlite3 -lgcrypt
debug-server: $(SERVER_MAIN) $(SERVER_OBJ) $(SHARED_OBJ)
	@g++ $(CXXDFLAGS) -Iinclude $^ -o bin/$@ -lsqlite3 -lgcrypt
run-server: $(SERVER_BIN)
	@./$(SERVER_BIN)
# ====================================== #

# ============= CLIENT CLI ============= #
$(CLI_BIN): $(CLI_MAIN) $(CLIENT_OBJ) $(CLI_OBJ) $(ASSETS_CLI_OBJ) $(SHARED_OBJ) lib/libCommunicationAPI.a
	@g++ $(CXXFLAGS) -Iinclude $^ -o $@ -lncursesw -lmenu
debug-cli: $(CLI_MAIN) $(CLIENT_OBJ) $(CLI_OBJ) $(ASSETS_CLI_OBJ) $(SHARED_OBJ) lib/libCommunicationAPI.a
	@g++ $(CXXDFLAGS) -Iinclude -g $^ -o bin/$@ -lncursesw -lmenu
run-cli: $(CLI_BIN)
	@./$(CLI_BIN)
# ====================================== #

# ============= CLIENT GUI ============= #  'GUI' name is set for GUI version
$(GUI_BIN): $(GUI_MAIN) $(CLIENT_OBJ) $(GUI_OBJ) $(ASSETS_GUI_OBJ) $(SHARED_OBJ) lib/libCommunicationAPI.a
	@g++ $(CXXFLAGS) -Iinclude $^ -o $@ -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -DGUI
debug-gui: $(GUI_MAIN) $(CLIENT_OBJ) $(GUI_OBJ) $(ASSETS_GUI_OBJ) $(SHARED_OBJ) lib/libCommunicationAPI.a
	@g++ $(CXXDFLAGS) -Iinclude $^ -o bin/$@ -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -DGUI
run-gui: $(GUI_BIN)
	@./$(GUI_BIN)
# ====================================== #

clean-server:
	@rm -rf $(SERVER_BIN) obj/server
clean-gui:
	@rm -rf $(GUI_BIN) obj/client/gui $(ASSETS_GUI)
clean-cli:
	@rm -rf $(CLI_BIN) obj/client/cli $(ASSETS_CLI)
clean-client:
	@make clean-gui >> /dev/null
	@make clean-cli >> /dev/null
clean-build:
	@make clean-server >> /dev/null
	@make clean-client >> /dev/null
	@rm -rf bin obj lib $(ASSETS_ID)
clean:
	@make clean-build >> /dev/null
	@rm -rf static/ltype.db static/built src/client/*/Assets.cpp

.PHONY: all run-server debug-server debug-cli run-cli debug-gui run-gui clean-server clean-gui clean-cli clean-client clean-build clean
