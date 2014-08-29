#ifndef PACKAGE_H_
#define PACKAGE_H_

#include "Stream.h"
#include "FileSystem.h"
#include <string>

namespace gameplay
{
    /**
     * Packages are used as a fallback solution for files that can't be normally opened by FileSystem::open.
     * Package is an abstract class. Client application can inherit from Package class to implement
     * different packages logic, i.e zip packages, web packages etc.
     */
    class Package
    {
    public:
        /**
        * Destructor.
        */
        virtual ~Package();

        /**
        * Opens a byte stream for the given resource path.
        *
        * If <code>path</code> is a file path, the file at the specified location is opened relative to the currently set
        * resource path.
        *
        * @param path The path to the resource to be opened, relative to the currently set resource path.
        * @param streamMode The stream mode used to open the file.
        *
        * @return A stream that can be used to read or write to the file depending on the mode.
        *         Returns NULL if there was an error. (Request mode not supported).
        *
        * @script{ignore}
        */
        virtual Stream* open(const char* path, size_t streamMode = FileSystem::READ) = 0;

        /**
        * Checks if the file at the given path exists.
        *
        * @param filePath The path to the file.
        *
        * @return <code>true</code> if the file exists; <code>false</code> otherwise.
        */
        virtual bool fileExists(const char* filePath) = 0;

    protected:

        /**
        * Constructor.
        */
        Package();
    };

}

#endif
