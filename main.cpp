#include "App.h"
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_main.h>

#ifdef __WINDOWS__
int SDL_main(int argc, char* argv[]) {
#else
int main( int argc, char * argv[] ) {
#endif

    srand(time(NULL));

	cApp app;

	if (app.InitApp())
		app.Run();

	app.CleanUp();

    return 1;
}
