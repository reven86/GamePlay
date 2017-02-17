/*
    Copyright 2016 Andrew Karpushin (reven86@gmail.com).

    This source file is a Derivative Work from orignal Gameplay3D source files.
    The Gameplay3D project is distributed under the terms of Apache 2.0 License.
    Original Gameplay3D source files can be found at https://github.com/gameplay3d/GamePlay

    Changes to orginal document were done in lines: 3, 11, 16, 39, 118, 252.
*/

#include "Base.h"
#include "MeshBatch.h"
#include "Material.h"
#include "Model.h"

namespace gameplay
{

MeshBatch::MeshBatch(const VertexFormat& vertexFormat, Mesh::PrimitiveType primitiveType, Material* material, bool indexed, unsigned int initialCapacity, unsigned int growSize)
    : _vertexFormat(vertexFormat), _primitiveType(primitiveType), _material(material), _indexed(indexed), _capacity(0), _growSize(growSize),
    _vertexCapacity(0), _indexCapacity(0), _vertexCount(0), _indexCount(0), _vertices(NULL), _verticesPtr(NULL), _indices(NULL), _indicesPtr(NULL), _started(false)
{
#ifdef EMSCRIPTEN
    _model = Model::create(Mesh::createMesh(vertexFormat, initialCapacity, true));
    _model->getMesh()->release();
#endif
    resize(initialCapacity);
}

MeshBatch::~MeshBatch()
{
#ifdef EMSCRIPTEN
    SAFE_RELEASE(_model);
#endif
    SAFE_RELEASE(_material);
    SAFE_DELETE_ARRAY(_vertices);
    SAFE_DELETE_ARRAY(_indices);
}

MeshBatch* MeshBatch::create(const VertexFormat& vertexFormat, Mesh::PrimitiveType primitiveType, const char* materialPath, bool indexed, unsigned int initialCapacity, unsigned int growSize)
{
    Material* material = Material::create(materialPath);
    if (material == NULL)
    {
        GP_ERROR("Failed to create material for mesh batch from file '%s'.", materialPath);
        return NULL;
    }
    MeshBatch* batch = create(vertexFormat, primitiveType, material, indexed, initialCapacity, growSize);
    SAFE_RELEASE(material); // batch now owns the material
    return batch;
}

MeshBatch* MeshBatch::create(const VertexFormat& vertexFormat, Mesh::PrimitiveType primitiveType, Material* material, bool indexed, unsigned int initialCapacity, unsigned int growSize)
{
    GP_ASSERT(material);

    MeshBatch* batch = new MeshBatch(vertexFormat, primitiveType, material, indexed, initialCapacity, growSize);
#ifdef EMSCRIPTEN
    batch->_model->setMaterial(material);
#endif

    material->addRef();

    return batch;
}

void MeshBatch::add(const void* vertices, size_t size, unsigned int vertexCount, const unsigned short* indices, unsigned int indexCount)
{
    GP_ASSERT(vertices);
    
    unsigned int newVertexCount = _vertexCount + vertexCount;
    unsigned int newIndexCount = _indexCount + indexCount;
    if (_primitiveType == Mesh::TRIANGLE_STRIP && _vertexCount > 0)
        newIndexCount += 2; // need an extra 2 indices for connecting strips with degenerate triangles
    
    // Do we need to grow the batch?
    while (newVertexCount > _vertexCapacity || (_indexed && newIndexCount > _indexCapacity))
    {
        if (_growSize == 0)
            return; // growing disabled, just clip batch
        if (!resize(_capacity + _growSize))
            return; // failed to grow
    }
    
    // Copy vertex data.
    GP_ASSERT(_verticesPtr);
    unsigned int vBytes = vertexCount * _vertexFormat.getVertexSize();
    memcpy(_verticesPtr, vertices, vBytes);
    
    // Copy index data.
    if (_indexed)
    {
        GP_ASSERT(indices);
        GP_ASSERT(_indicesPtr);

        if (_vertexCount == 0)
        {
            // Simply copy values directly into the start of the index array.
            memcpy(_indicesPtr, indices, indexCount * sizeof(unsigned short));
        }
        else
        {
            if (_primitiveType == Mesh::TRIANGLE_STRIP)
            {
                // Create a degenerate triangle to connect separate triangle strips
                // by duplicating the previous and next vertices.
                _indicesPtr[0] = *(_indicesPtr-1);
                _indicesPtr[1] = _vertexCount;
                _indicesPtr += 2;
            }
            
            // Loop through all indices and insert them, with their values offset by
            // 'vertexCount' so that they are relative to the first newly inserted vertex.
            for (unsigned int i = 0; i < indexCount; ++i)
            {
                _indicesPtr[i] = indices[i] + _vertexCount;
            }
        }
        _indicesPtr += indexCount;
        _indexCount = newIndexCount;
    }
    
    _verticesPtr += vBytes;
    _vertexCount = newVertexCount;
}

void MeshBatch::updateVertexAttributeBinding()
{
    GP_ASSERT(_material);

    // Update our vertex attribute bindings.
    for (unsigned int i = 0, techniqueCount = _material->getTechniqueCount(); i < techniqueCount; ++i)
    {
        Technique* t = _material->getTechniqueByIndex(i);
        GP_ASSERT(t);
        for (unsigned int j = 0, passCount = t->getPassCount(); j < passCount; ++j)
        {
            Pass* p = t->getPassByIndex(j);
            GP_ASSERT(p);
#ifdef EMSCRIPTEN
            VertexAttributeBinding* b = VertexAttributeBinding::create(_model->getMesh(), p->getEffect());
#else
            VertexAttributeBinding* b = VertexAttributeBinding::create(_vertexFormat, _vertices, p->getEffect());
#endif
            p->setVertexAttributeBinding(b);
            SAFE_RELEASE(b);
        }
    }
}

unsigned int MeshBatch::getCapacity() const
{
    return _capacity;
}

void MeshBatch::setCapacity(unsigned int capacity)
{
    resize(capacity);
}

bool MeshBatch::resize(unsigned int capacity)
{
    if (capacity == 0)
    {
        GP_ERROR("Invalid resize capacity (0).");
        return false;
    }

    if (capacity == _capacity)
        return true;

    // Store old batch data.
    unsigned char* oldVertices = _vertices;
    unsigned short* oldIndices = _indices;

    unsigned int vertexCapacity = 0;
    switch (_primitiveType)
    {
    case Mesh::LINES:
        vertexCapacity = capacity * 2;
        break;
    case Mesh::LINE_STRIP:
        vertexCapacity = capacity + 1;
        break;
    case Mesh::POINTS:
        vertexCapacity = capacity;
        break;
    case Mesh::TRIANGLES:
        vertexCapacity = capacity * 3;
        break;
    case Mesh::TRIANGLE_STRIP:
        vertexCapacity = capacity + 2;
        break;
    default:
        GP_ERROR("Unsupported primitive type for mesh batch (%d).", _primitiveType);
        return false;
    }

    // We have no way of knowing how many vertices will be stored in the batch
    // (we only know how many indices will be stored). Assume the worst case
    // for now, which is the same number of vertices as indices.
    unsigned int indexCapacity = vertexCapacity;
    if (_indexed && indexCapacity > USHRT_MAX)
    {
        GP_ERROR("Index capacity is greater than the maximum unsigned short value (%d > %d).", indexCapacity, USHRT_MAX);
        return false;
    }

    // Allocate new data and reset pointers.
    unsigned int voffset = _verticesPtr - _vertices;
    unsigned int vBytes = vertexCapacity * _vertexFormat.getVertexSize();
    _vertices = new unsigned char[vBytes];
    if (voffset >= vBytes)
        voffset = vBytes - 1;
    _verticesPtr = _vertices + voffset;

    if (_indexed)
    {
        unsigned int ioffset = _indicesPtr - _indices;
        _indices = new unsigned short[indexCapacity];
        if (ioffset >= indexCapacity)
            ioffset = indexCapacity - 1;
        _indicesPtr = _indices + ioffset;
    }

    // Copy old data back in
    if (oldVertices)
        memcpy(_vertices, oldVertices, std::min(_vertexCapacity, vertexCapacity) * _vertexFormat.getVertexSize());
    SAFE_DELETE_ARRAY(oldVertices);
    if (oldIndices)
        memcpy(_indices, oldIndices, std::min(_indexCapacity, indexCapacity) * sizeof(unsigned short));
    SAFE_DELETE_ARRAY(oldIndices);

    // Assign new capacities
    _capacity = capacity;
    _vertexCapacity = vertexCapacity;
    _indexCapacity = indexCapacity;

    // Update our vertex attribute bindings now that our client array pointers have changed
    updateVertexAttributeBinding();

    return true;
}

void MeshBatch::add(const float* vertices, unsigned int vertexCount, const unsigned short* indices, unsigned int indexCount)
{
    add(vertices, sizeof(float), vertexCount, indices, indexCount);
}

void MeshBatch::start()
{
    _vertexCount = 0;
    _indexCount = 0;
    _verticesPtr = _vertices;
    _indicesPtr = _indices;
    _started = true;
}

bool MeshBatch::isStarted() const
{
    return _started;
}

void MeshBatch::finish()
{
#ifdef EMSCRIPTEN
    _model->getMesh()->setVertexData(reinterpret_cast<const float*>(_vertices), 0, _vertexCount);
#endif
    _started = false;
}

void MeshBatch::draw() const
{
    if (_vertexCount == 0 || (_indexed && _indexCount == 0))
        return; // nothing to draw

    // Not using VBOs, so unbind the element array buffer.
    // ARRAY_BUFFER will be unbound automatically during pass->bind().
    GL_ASSERT( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 ) );

#ifdef EMSCRIPTEN
    if (!_indexed)
    {
        _model->draw();
        return;
    }
#endif

    GP_ASSERT(_material);
    if (_indexed)
        GP_ASSERT(_indices);

    // Bind the material.
    Technique* technique = _material->getTechnique();
    GP_ASSERT(technique);
    unsigned int passCount = technique->getPassCount();
    for (unsigned int i = 0; i < passCount; ++i)
    {
        Pass* pass = technique->getPassByIndex(i);
        GP_ASSERT(pass);
        pass->bind();

        if (_indexed)
        {
            GL_ASSERT( glDrawElements(_primitiveType, _indexCount, GL_UNSIGNED_SHORT, (GLvoid*)_indices) );
        }
        else
        {
            GL_ASSERT( glDrawArrays(_primitiveType, 0, _vertexCount) );
        }

        pass->unbind();
    }
}

void MeshBatch::erase(unsigned int vertexCount)
{
    GP_ASSERT(!_indexed);
    if (_vertexCount > vertexCount)
        _vertexCount -= vertexCount;
    else
        _vertexCount = 0;
    _verticesPtr = _vertices + _vertexCount * _vertexFormat.getVertexSize();
}

}
