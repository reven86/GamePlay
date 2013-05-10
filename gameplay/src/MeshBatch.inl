#include "MeshBatch.h"

namespace gameplay
{

Material* MeshBatch::getMaterial() const
{
    return _material;
}

template <class T>
void MeshBatch::add(const T* vertices, unsigned int vertexCount, const unsigned short* indices, unsigned int indexCount)
{
    GP_ASSERT(sizeof(T) == _vertexFormat.getVertexSize());
    add(vertices, sizeof(T), vertexCount, indices, indexCount);
}

template <class T>
T * MeshBatch::reserve(unsigned int vertexCount)
{
    GP_ASSERT(sizeof(T) == _vertexFormat.getVertexSize());
    GP_ASSERT(!_indexed);
    
    unsigned int newVertexCount = _vertexCount + vertexCount;
    
    // Do we need to grow the batch?
    while (newVertexCount > _vertexCapacity)
    {
        if (_growSize == 0)
            return NULL; // growing disabled, just clip batch
        if (!resize(_capacity + _growSize))
            return NULL; // failed to grow
    }
    
    // Copy vertex data.
    GP_ASSERT(_verticesPtr);
    unsigned int vBytes = vertexCount * _vertexFormat.getVertexSize();

    unsigned char* oldPtr = _verticesPtr;
        
    _verticesPtr += vBytes;
    _vertexCount = newVertexCount;

    return reinterpret_cast< T * >( oldPtr );
}

}
