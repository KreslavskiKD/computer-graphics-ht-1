#ifndef MORPHING_BUFFERS
#define MORPHING_BUFFERS

#include <QOpenGLBuffer>

#include <tinygltf/tiny_gltf.h>

struct BufferVbo {
    BufferVbo(const tinygltf::Buffer& buf, const tinygltf::BufferView& bufView) {
        glBuffer_.create();
        bind();
        glBuffer_.setUsagePattern(QOpenGLBuffer::StaticDraw);
        glBuffer_.allocate(buf.data.data() + static_cast<int>(bufView.byteOffset), static_cast<int>(bufView.byteLength));
    }

    void bind() {
        glBuffer_.bind();
    }

    ~BufferVbo() {
        glBuffer_.release();
    }

private:
    QOpenGLBuffer glBuffer_{QOpenGLBuffer::Type::VertexBuffer};
};

struct BufferEbo {
    BufferEbo(const tinygltf::Buffer& buf, const tinygltf::BufferView& bufView) {
        glBuffer_.create();
        bind();
        glBuffer_.setUsagePattern(QOpenGLBuffer::StaticDraw);
        glBuffer_.allocate(buf.data.data() + static_cast<int>(bufView.byteOffset), static_cast<int>(bufView.byteLength));
    }

    void bind() {
        glBuffer_.bind();
    }

    ~BufferEbo() {
        glBuffer_.release();
    }

private:
    QOpenGLBuffer glBuffer_{QOpenGLBuffer::Type::IndexBuffer};
};

#endif // MORPHING_BUFFERS

