#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include "Ref.h"

namespace gameplay
{

class Node;
class NodeCloneContext;
class MeshSkin;
class Animation;
class BoundingSphere;
class BoundingBox;
class Terrain;

/**
 * Defines a drawable object that can be attached to a Node.
 */
class Drawable : public virtual Ref
{
    friend class Node;

public:

    /**
     * Constructor.
     */
    Drawable();

    /**
     * Destructor.
     */
    virtual ~Drawable();

    /**
     * Draws the object.
     *
     * @param wireframe true if you want to request to draw the wireframe only.
     * @return The number of graphics draw calls required to draw the object.
     */

    virtual unsigned int draw(bool wireframe = false) const = 0;

    /**
     * Gets the node this drawable is attached to.
     *
     * @return The node this drawable is attached to.
     */
    Node* getNode() const;

    /**
     * Get MeshSkin associated with this drawable, if any.
     */
    virtual const MeshSkin* getSkin() const { return nullptr; };

    /**
     * Get Terrain associated with this drawable, if any.
     */
    virtual const Terrain* getTerrain() const { return nullptr; };

    /**
     * Gets the first animation in the node hierarchy with the specified ID.
     *
     * @param id The ID of the animation to get. Returns the first animation if ID is NULL.
     * @return The first animation with the specified ID.
     */
    virtual Animation* getAnimation(const char * id) const;

    /**
     * Gets the local bounding sphere for this drawable.
     *
     * @return True if the drawable has bounding sphere.
     */
    virtual bool getBoundingSphere(BoundingSphere* outSphere) const { return false; };

    /**
     * Gets the local bounding box for this drawable.
     *
     * @return True if the drawable has bounding box.
     */
    virtual bool getBoundingBox(BoundingBox* outBox) const { return false; };

protected:

    /**
     * Clones the drawable and returns a new drawable.
     *
     * @param context The clone context.
     * @return The newly created drawable.
     */
    virtual Drawable* clone(NodeCloneContext& context) = 0;

    /**
     * Sets the node this drawable is attached to.
     *
     * @param node The node this drawable is attached to.
     */
    virtual void setNode(Node* node);

    /**
     * Node this drawable is attached to.
     */
    Node* _node;
};

}

#endif
