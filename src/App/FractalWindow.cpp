#include "FractalWindow.h"

#include <QLabel>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QScreen>
#include <QVBoxLayout>

#include <array>
#include <string>

namespace {

constexpr std::array<GLfloat, 8u> vertices = {
	-1.0f,
	-1.0f,
	-1.0f,
	1.0f,
	1.0f,
	-1.0f,
	1.0f,
	1.0f,
};
constexpr std::array<GLuint, 6u> indices = {0, 1, 2, 1, 2, 3};

}// namespace

void FractalWindow::init() {
	m_time.start();

	// Configure shaders
	program_ = std::make_unique<QOpenGLShaderProgram>(this);
	program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/diffuse.vs");
	program_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/diffuse.fs");
	program_->link();

	// Create VAO object
	vao_.create();
	vao_.bind();

	// Create VBO
	vbo_.create();
	vbo_.bind();
	vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo_.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(GLfloat)));

	// Create IBO
	ibo_.create();
	ibo_.bind();
	ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	ibo_.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(GLuint)));

	// Bind attributes
	program_->bind();

	program_->enableAttributeArray(0);
	program_->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(2 * sizeof(GLfloat)));

	iterationsUniform_ = program_->uniformLocation("iterations");
	param1Uniform_ = program_->uniformLocation("param1");
	param2Uniform_ = program_->uniformLocation("param2");
	param3Uniform_ = program_->uniformLocation("param3");
	zoomUniform_ = program_->uniformLocation("zoom");
	shiftUniform_ = program_->uniformLocation("shift");

	// Release all
	program_->release();

	vao_.release();

	ibo_.release();
	vbo_.release();

	// Uncomment to enable depth test and face culling
	// glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);

	// Clear all FBO buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FractalWindow::render() {
	// Configure viewport
	const auto retinaScale = devicePixelRatio();
	glViewport(0, 0, static_cast<GLint>(width() * retinaScale),
			   		static_cast<GLint>(height() * retinaScale));

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind VAO and shader program
	program_->bind();
	vao_.bind();

	// Update uniform value
	program_->setUniformValue(iterationsUniform_, iterations_);
	program_->setUniformValue(param1Uniform_, param1_);
	program_->setUniformValue(param2Uniform_, param2_);
	program_->setUniformValue(param3Uniform_, param3_);
	program_->setUniformValue(zoomUniform_, zoom_);
	program_->setUniformValue(shiftUniform_, globalShift_ + shift_);

	// Draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	// Release VAO and shader program
	vao_.release();
	program_->release();

	// Increment frame counter
	if (m_time.elapsed() >= 1000) {
		const auto elapsedSeconds = static_cast<float>(m_time.restart()) / 1000.0f;
		fps = static_cast<size_t>(std::round(frame_ / elapsedSeconds));
		std::string str = std::to_string(fps);
		if (fpsLabelValue_ != nullptr) {
			fpsLabelValue_->setText(QString::fromUtf8(str.c_str()));
		}
		frame_ = 0;
	}
	++frame_;
}

void FractalWindow::destroy() {
	program_.reset();
}

void FractalWindow::mousePressEvent(QMouseEvent * e) {
	isPressed_ = true;
	mousePressPosition_ = QVector2D(e->localPos());
}

void FractalWindow::mouseReleaseEvent(QMouseEvent * e) {
	isPressed_ = false;
	int xAtRelease = e->x();
	int yAtRelease = e->y();
	float dx = xAtRelease - mousePressPosition_.x();
	float dy = yAtRelease - mousePressPosition_.y();
	globalShift_ += QVector2D(-2 * dx / (float)width(), 2 * dy / (float)height());
	shift_ = QVector2D(0, 0);
}

void FractalWindow::mouseMoveEvent(QMouseEvent * e) {
	if (isPressed_) {
		int xAtMove = e->x();
		int yAtMove = e->y();
		float dx = xAtMove - mousePressPosition_.x();
		float dy = yAtMove - mousePressPosition_.y();
		shift_ = QVector2D(-2 * dx / (float)width(), 2 * dy / (float)height());
	}
}

void FractalWindow::wheelEvent(QWheelEvent * e) {
	float prev = zoom_;
	float x = float(e->position().x() / width());
	float y = 1.0f - float(e->position().y() / height());
	zoom_ = std::max(0.1f, zoom_ + float(e->angleDelta().y() / 1000.) * zoom_);

	globalShift_ = zoom_ / prev * (QVector2D(-1, -1) + globalShift_ + 2 * QVector2D(x, y))
		- QVector2D(-1, -1) - 2 * QVector2D(x, y);
}

void FractalWindow::setIterations(int iterations) {
	iterations_ = iterations;
}


void FractalWindow::setParam1(float param1) {
	param1_ = param1;
}

void FractalWindow::setParam2(float param2) {
	param2_ = param2;
}

void FractalWindow::setParam3(float param3) {
	param3_ = param3;
}

void FractalWindow::setFpsCounter(QLabel * fpsLabelValue) {
	fpsLabelValue_ = fpsLabelValue;
}
