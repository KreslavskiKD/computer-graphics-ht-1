#pragma once

#include "Utils/Buffers.h"
#include "Utils/Drawable.h"
#include "Utils/Camera.h"
#include "Utils/FpsGuard.h"

#include <tinygltf/tiny_gltf.h>

#include <Base/GLWidget.hpp>

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QQuaternion>
#include <QVector2D>
#include <QVector3D>
#include <QElapsedTimer>
#include <QTime>
#include <QLabel>
#include <QSlider>

#include <memory>
#include <unordered_map>


class MorphingWindow final :  public fgl::GLWidget
{
	Q_OBJECT
public:
	MorphingWindow() noexcept;
	~MorphingWindow() override;

	void onInit() override;
	void onRender() override;
	void onResize(size_t width, size_t height) override;

	void keyReleaseEvent(QKeyEvent *event) override;

signals:
	void updateUI();

private:
	void setAllBuffers();

	void bindMesh(tinygltf::Mesh &mesh);
	void bindModelNodes(tinygltf::Node &node);
	void bindModel();

	// void drawModelNodes(tinygltf::Node &node);
	// void drawMesh(tinygltf::Mesh &mesh);

	bool loadModel(const char *filename);

	[[nodiscard]] FpsGuard captureMetrics();

	QLabel * fpsLabelValue_;

	QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
	QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
	QOpenGLVertexArrayObject vao_;

	morphing::Camera cam;
	GLint mUniform_ = -1;
	GLint vUniform_ = -1;
	GLint pUniform_ = -1;
	GLint lightColorUniform_ = -1;
	GLint lightPosUniform_ = -1;
	GLint viewPosUniform_ = -1;
	GLint morphingUniform_ = -1;
	QVector3D morphing_{1., 0., 0.}; 
	QSlider morphingSlider = QSlider(Qt::Horizontal);

    std::vector<Drawable> drawables;

	QMatrix4x4 model_;
	QMatrix4x4 view_;
	QMatrix4x4 projection_;

	std::unique_ptr<QOpenGLShaderProgram> program_;
	

	QElapsedTimer timer_;
	size_t frameCount_ = 0;

	struct {
		size_t fps = 0;
	} ui_;

	bool animated_ = true;

	tinygltf::Model model;
	std::vector<BufferVbo> vbos;
    std::vector<BufferEbo> ebos;

	std::map<int, GLuint> buffer_to_vbo;
	std::map<int, GLuint> buffer_to_ebo;

};
