#ifndef STORECONTROLLER_H_
#define STORECONTROLLER_H_

#include "StoreFront.h"

namespace gameplay
{

/**
 * Defines a class for controlling various back-end support for store capabilities.
 *
 * @script{ignore}
 */
class StoreController
{
    friend class Game;

public:
    StoreFront * getStoreFront( ) const {return _storeFront;};

private:

    /**
     * Constructor.
     */
    StoreController();

    /**
     * Destructor.
     */
    virtual ~StoreController();

    /**
     * Callback for when the controller is initialized.
     */
    void initialize();

    /*
     * Callback for when the controller is finalized.
     */
    void finalize();

    /**
     * Pauses the controller.
     */
    void pause();

    /**
     * Resumes the controller.
     */
    void resume();

    /**
     * Callback for when the controller receives a frame update event.
     */
    void update(float elapsedTime);

private:
    StoreFront * _storeFront;
};

}

#endif

