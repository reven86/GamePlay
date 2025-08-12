#include "Base.h"
#include "Drawable.h"
#include "MeshSkin.h"


namespace gameplay
{

Drawable::Drawable()
    : _node(NULL)
{
}

Drawable::~Drawable()
{
}

Node* Drawable::getNode() const
{
    return _node;
}

void Drawable::setNode(Node* node)
{
    _node = node;
}

Animation* Drawable::getAnimation(const char* id) const
{
    // Check to see if there's any animations with the ID on the joints.
    const MeshSkin* skin = getSkin();
    if (skin)
    {
        Animation * animation = skin->getAnimation(id);
        if (animation)
            return animation;
    }

    return nullptr;
}

}
