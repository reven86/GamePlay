#ifndef GP_NO_PLATFORM
#ifdef __APPLE__

#import <Foundation/Foundation.h>
#include "gameplay.h"


bool validateReceipt( );


using namespace gameplay;

extern int __argc;
extern char** __argv;

/**
 * Main entry point.
 */
int main(int argc, char** argv)
{
    __argc = argc;
    __argv = argv;
    
    if( !validateReceipt( ) )
        return 0;
    
    NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
    Game* game = Game::getInstance();
    Platform* platform = Platform::create(game);
    GP_ASSERT(platform);
    int result = platform->enterMessagePump();
    delete platform;
    [p release];
    return result;
}

#endif
#endif