#ifndef MORPHING_DRAWABLE
#define MORPHING_DRAWABLE

#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>

#include <memory>

struct Drawable {

	Drawable() {
		vao->create();
	};

    std::unique_ptr<QOpenGLVertexArrayObject> vao = std::make_unique<QOpenGLVertexArrayObject>();
	std::unique_ptr<QOpenGLTexture> texture;
	bool ebo = false;

    int primitiveMode;
	size_t indexAccessorCount;
	int indexAccessorComponentType;
    size_t indexAccessorByteOffset;

    void draw()  {
        static QOpenGLFunctions funcs;
        static bool inited = false;
        if (!inited) {
            funcs.initializeOpenGLFunctions();
            inited = true;
        }
 
        vao->bind();
        funcs.glActiveTexture(GL_TEXTURE0);
        texture->bind();

        if (ebo) {
            funcs.glDrawElements(
                primitiveMode, 
                static_cast<GLsizei>(indexAccessorCount),
                indexAccessorComponentType,
                ((char *)NULL + (indexAccessorByteOffset))
            );
        } else {
            funcs.glDrawArrays(
                primitiveMode,
                0,
                static_cast<GLsizei>(indexAccessorCount)
            );
        }

        texture->release();
        vao->release();
    }

};

#endif //MORPHING_DRAWABLE
