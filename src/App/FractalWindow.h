#pragma once

#include <Base/GLWindow.hpp>

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

#include <memory>

class FractalWindow final : public fgl::GLWindow
{

public:
	void init() override;
	void render() override;
	void destroy() override;
	void setIterations(int iterations);
	void setParam1(float param1);
	void setParam2(float param2);
	void setParam3(float param3);
	void setFpsCounter(QLabel * fpsLabelValue);

protected:
	void mousePressEvent(QMouseEvent * e) override;
	void mouseReleaseEvent(QMouseEvent * e) override;
	void mouseMoveEvent(QMouseEvent * e) override;
	void wheelEvent(QWheelEvent * e) override;

private:
	GLint shiftUniform_ = -1;
	GLint zoomUniform_ = -1;
	GLint iterationsUniform_ = -1;
	GLint param1Uniform_ = -1;
	GLint param2Uniform_ = -1;
	GLint param3Uniform_ = -1;

	int iterations_ = 100;
	float param1_ = 2.0;
	float param2_ = (float)(-0.345);
	float param3_ = (float)0.654;
	float zoom_ = (float)0.4;
	QVector2D shift_{0., 0.};

	QLabel * fpsLabelValue_;

	QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
	QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
	QOpenGLVertexArrayObject vao_;

	std::unique_ptr<QOpenGLShaderProgram> program_ = nullptr;

	size_t frame_ = 0;
	QElapsedTimer m_time;
	float fps = 0;

	QVector2D mousePressPosition_{0., 0.};
	bool isPressed_ = false;
	QVector2D globalShift_{0., 0.};
};
