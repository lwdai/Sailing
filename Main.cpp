#include "sailing.hpp"

#include <iostream>
using namespace std;

int main( int argc, char **argv ) 
{
	if (argc > 1) {
		std::string luaSceneFile(argv[1]);
		std::string title("Project: SAILING - [");
		title += luaSceneFile;
		title += "]";

		CS488Window::launch(argc, argv, new Sailing(luaSceneFile), 1024, 768, title);

	} else {
		cout << "Must supply Lua file as first argument to program.\n";
        cout << "For example:\n";
        cout << "./SAILING Assets/simpleScene.lua\n";
	}

	return 0;
}
