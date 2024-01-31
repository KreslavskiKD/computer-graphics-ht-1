#include "MorphingWindow.h"

#include <QLabel>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QScreen>
#include <QVBoxLayout>
#include <QtMath>

#include <array>
#include <string>
#include <filesystem>
#include <iostream>

// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define JSON_NOEXCEPTION
#define TINYGLTF_NOEXCEPTION

MorphingWindow::MorphingWindow() noexcept {
	const auto formatFPS = [](const auto value, const auto morphing) {
		return QString("FPS: %1, morphing: %2").arg(QString::number(value), QString::number(morphing));
	};

	morphingSlider.setRange(0, 100);
	morphingSlider.setValue(0);

	auto fps = new QLabel(formatFPS(0, 1), this);
	fps->setStyleSheet("QLabel { color : white; }");

	auto layout = new QVBoxLayout();
	layout->addWidget(fps, 1);
	layout->addWidget(&morphingSlider);

	setLayout(layout);

	timer_.start();

	connect(this, &MorphingWindow::updateUI, [=] {
		fps->setText(formatFPS(ui_.fps, morphingSlider.value()));
	});
	connect(&morphingSlider, SIGNAL(valueChanged(int)), &morphingSlider, SLOT(setValue(int)));
}

MorphingWindow::~MorphingWindow() {
	{
		const auto guard = bindContext();
		program_.reset();
	}
}

bool MorphingWindow::loadModel(const char *filename) {
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
	if (!warn.empty()) {
		std::cout << "Warn: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "Err: " << err << std::endl;
	}

	if (!ret) {
		std::cout << "Failed to load glTF: " << filename << std::endl;
	} else {
		std::cout << "Loaded glTF: " << filename << std::endl;
	}

	return ret;
}


void MorphingWindow::onInit() {
	// Load GLTF model
	const std::filesystem::path path = std::filesystem::absolute("../../../../src/App/Models/18th-century-oilan/source/OilCan.glb");

	if (!loadModel(path.string().c_str())) {
		return;
	}

	// Configure shaders
	program_ = std::make_unique<QOpenGLShaderProgram>(this);
	program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/diffuse.vs");
	program_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/diffuse.fs");
	program_->link();
	program_->bind();

	// Set buffers and bind Model
	setAllBuffers();
	bindModel();

	mUniform_ = program_->uniformLocation("model");
	vUniform_ = program_->uniformLocation("view");
	pUniform_ = program_->uniformLocation("projection");
  
	morphingUniform_ = program_->uniformLocation("morphing");
	lightColorUniform_ = program_->uniformLocation("lightColor");
	lightPosUniform_ = program_->uniformLocation("lightPos");
	viewPosUniform_ = program_->uniformLocation("viewPos");

	// Release all
	program_->release();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Clear all FBO buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MorphingWindow::onRender() {
	const auto guard = captureMetrics();

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Calculate MVP matrix
	model_.setToIdentity();

	view_.setToIdentity();
	view_.translate(0, -2, -7);

	// Bind VAO and shader program
	program_->bind();

	// Update uniform value
	program_->setUniformValue(mUniform_, model_);
	program_->setUniformValue(vUniform_, cam.getView());
	program_->setUniformValue(pUniform_, projection_);
	program_->setUniformValue(lightColorUniform_, QVector3D(1., static_cast<float>(0.8), 1.));
	program_->setUniformValue(lightPosUniform_, QVector3D(5., 5., 5.));
	program_->setUniformValue(viewPosUniform_, cam.getPos());

	morphing_.setX((double)morphingSlider.value() / 100);
	program_->setUniformValue(morphingUniform_, morphing_);

	// Draw
 	for (auto& m : drawables) {
    	m.draw();
	}

	// Release VAO and shader program
	program_->release();

	++frameCount_;

	if (animated_) {
		update();
	}
}

void MorphingWindow::keyReleaseEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_E) {
		cam.moveForward();
    }
	if (event->key() == Qt::Key_F) {
        cam.moveBackward();
    }
	if (event->key() == Qt::Key_W) {
		cam.moveUp();
    }
	if(event->key() == Qt::Key_S) {
		cam.moveDown();
    }
	if(event->key() == Qt::Key_A) {
		cam.moveLeft();
    }
	if(event->key() == Qt::Key_D) {
		cam.moveRight();
	}
}

void MorphingWindow::onResize(const size_t width, const size_t height)
{
	// Configure viewport
	glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));

	// Configure matrix
	const auto aspect = static_cast<float>(width) / static_cast<float>(height);
	const auto zNear = 0.1f;
	const auto zFar = 100.0f;
	const auto fov = 60.0f;
	projection_.setToIdentity();
	projection_.perspective(fov, aspect, zNear, zFar);
}

void MorphingWindow::bindMesh(tinygltf::Mesh &mesh) {
	std::cout << "--> bindMesh " << std::endl;

	for (size_t i = 0; i < mesh.primitives.size(); ++i) {
		std::cout << "--> bindMesh for, i = " << i << std::endl;
		drawables.emplace_back();
		drawables.back().vao->bind();

		std::cout << "--> 1 " << std::endl;

		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

		std::cout << "--> 2 " << std::endl;

		if (indexAccessor.bufferView >= 0) {

			std::cout << "--> 3 " << std::endl;

			assert(buffer_to_ebo.find(indexAccessor.bufferView) != buffer_to_ebo.end()); 

			std::cout << "--> 4 " << std::endl;

			ebos[buffer_to_ebo[indexAccessor.bufferView]].bind();

			std::cout << "--> 5 " << std::endl;

			drawables.back().ebo = true;

			std::cout << "--> 6 " << std::endl;
		}
		std::cout << "--> bind ebo: " << indexAccessor.bufferView << std::endl;

		for (auto &attrib : primitive.attributes) {
			tinygltf::Accessor accessor = model.accessors[attrib.second];
			int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
     
			std::cout << "assert:  " <<  accessor.bufferView<< std::endl;

			std::cout << "--> bind vbo: " << accessor.bufferView << std::endl;

			int size = 1;
			if (accessor.type != TINYGLTF_TYPE_SCALAR) {
				size = accessor.type;
			}

			int vaa = -1;
			if (attrib.first.compare("POSITION") == 0) {
				vaa = 0;
			}
			if (attrib.first.compare("NORMAL") == 0) {
				vaa = 1;
			}
			if (attrib.first.compare("TEXCOORD_0") == 0) {
				vaa = 2;
			}
			if (vaa > -1) {
				std::cout << "--> enableAttributeArray: " << vaa << std::endl;

				assert(buffer_to_vbo.find(accessor.bufferView) != buffer_to_vbo.end()); 
				vbos[buffer_to_vbo[accessor.bufferView]].bind();

				program_->enableAttributeArray(vaa);
				program_->setAttributeBuffer(vaa, accessor.componentType, static_cast<int>(accessor.byteOffset), size, byteStride);
			} else {
				std::cout << "vaa missing: " << attrib.first << std::endl;
			}
		}

		if (model.textures.size() > 0) {
			tinygltf::Texture &tex = model.textures[0];

			if (tex.source > -1) {
				tinygltf::Image &image = model.images[tex.source];

				GLenum format = GL_RGBA;

				if (image.component == 1) {
					format = GL_RED;
				} else if (image.component == 2) {
					format = GL_RG;
				} else if (image.component == 3) {
					format = GL_RGB;
				} 

				GLenum type = GL_UNSIGNED_BYTE;
				if (image.bits == 8) {
					type = GL_UNSIGNED_BYTE;
				} else if (image.bits == 16) {
					type = GL_UNSIGNED_SHORT;
				} 

				QImage im(image.image.data(), image.width, image.height, QImage::Format::Format_RGB32);
				drawables.back().texture = std::make_unique<QOpenGLTexture>(std::move(im));
				drawables.back().texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
				drawables.back().texture->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
			}
		}
		drawables.back().indexAccessorCount = indexAccessor.count;
		drawables.back().indexAccessorComponentType = indexAccessor.componentType;
		drawables.back().indexAccessorByteOffset = indexAccessor.byteOffset;
		drawables.back().primitiveMode = primitive.mode;
		drawables.back().vao->release();
	}
}

void MorphingWindow::bindModelNodes(tinygltf::Node &node) {
    if ((0 <= node.mesh) && (node.mesh < model.meshes.size())) {
		std::cout << "--> build node: " << node.mesh << std::endl;
        auto &mesh = model.meshes[node.mesh];
        bindMesh(mesh);
    }
    for (auto childId : node.children) {
		assert((node.children[childId] >= 0) && (node.children[childId] < model.nodes.size()));
        auto &child = model.nodes[childId];
        bindModelNodes(child);
    }
}

void MorphingWindow::bindModel() {
	const tinygltf::Scene &scene = model.scenes[model.defaultScene];
	for (auto nodeId : scene.nodes) {
		assert((scene.nodes[nodeId] >= 0) && (scene.nodes[nodeId] < model.nodes.size()));
		bindModelNodes(model.nodes[scene.nodes[nodeId]]); 
	}
}

// void MorphingWindow::drawModelMeshes(tinygltf::Mesh &mesh) {
//     for (auto &primitive : mesh.primitives) {
//         auto &accessor = model.accessors[primitive.indices];
//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebos.at(accessor.bufferView));
//         glDrawElements(primitive.mode, accessor.count, accessor.componentType, static_cast<char *>(nullptr) + accessor.byteOffset);
//     }
// }

// void MorphingWindow::drawModelNodes(tinygltf::Node &node) {
//     auto &mesh = model.meshes[node.mesh];
//     drawModelMeshes(ebos, mesh);
//     for (auto &childId : node.children) {
//         auto &child = model.nodes[childId];
//         drawModelNodes(ebos, child);
//     }
// }

void MorphingWindow::setAllBuffers() {
	vbos.reserve(model.bufferViews.size());
	ebos.reserve(model.bufferViews.size());

	int i = 0;
	for (auto bufferView : model.bufferViews) {
		std::cout << i << " " << bufferView.target<< std::endl;
		if (bufferView.target == 0) {  
			buffer_to_vbo[i] = static_cast<int>(vbos.size());
			vbos.emplace_back(model.buffers[bufferView.buffer], bufferView);
			i++;
			continue;
		}

		std::cout << "--> load buffer: " << i << std::endl;
		if (bufferView.target == 34962) { // ARRAY_BUFFER
			buffer_to_vbo[i] = static_cast<int>(vbos.size());
			vbos.emplace_back(model.buffers[bufferView.buffer], bufferView);
			i++;
			continue;
		}

		if (bufferView.target == 34963) { // ELEMENT_ARRAY_BUFFER
			buffer_to_ebo[i] = static_cast<int>(ebos.size());
			ebos.emplace_back(model.buffers[bufferView.buffer], bufferView);
			i++;
			continue;
		}
		assert(false && "unreachable");
	}
}

auto MorphingWindow::captureMetrics() -> FpsGuard {
	return FpsGuard{
		[&] {
			if (timer_.elapsed() >= 1000)
			{
				const auto elapsedSeconds = static_cast<float>(timer_.restart()) / 1000.0f;
				ui_.fps = static_cast<size_t>(std::round(frameCount_ / elapsedSeconds));
				frameCount_ = 0;
				emit updateUI();
			}
		}
	};
}
