#include "FractalWidget.h"
#include <QHBoxLayout>
#include <QSlider>

FractalWidget::FractalWidget(QWidget * parent)
	: QWidget(parent)
{
	QGridLayout * grid = new QGridLayout();

	iterationsLabel_ = new QLabel("Iterations: ", this);
	iterationsEdit = new QSlider(this);
	iterationsEdit->setOrientation(Qt::Horizontal);
	iterationsEdit->setMinimum(0);
	iterationsEdit->setMaximum(200);
	iterationsEdit->setValue(100);

	param1Label = new QLabel("Parameter 1: ", this);
	param1Edit = new QSlider(this);
	param1Edit->setOrientation(Qt::Horizontal);
	param1Edit->setMinimum(-1000);
	param1Edit->setMaximum(1000);
	param1Edit->setValue(0);

	param2Label = new QLabel("Parameter 2: ", this);
	param2Edit = new QSlider(this);
	param2Edit->setOrientation(Qt::Horizontal);
	param2Edit->setMinimum(-1000);
	param2Edit->setMaximum(1000);
	param2Edit->setValue(0);

	param3Label = new QLabel("Parameter 3: ", this);
	param3Edit = new QSlider(this);
	param3Edit->setOrientation(Qt::Horizontal);
	param3Edit->setMinimum(-1000);
	param3Edit->setMaximum(1000);
	param3Edit->setValue(0);

	fpsLabel_ = new QLabel("FPS: ", this);
	fpsLabelValue_ = new QLabel(QString::number(0.0), this);

	grid->addWidget(iterationsLabel_, 0, 0);
	grid->addWidget(iterationsEdit, 0, 1);

	grid->addWidget(param1Label, 2, 0);
	grid->addWidget(param1Edit, 2, 1);

	grid->addWidget(param2Label, 3, 0);
	grid->addWidget(param2Edit, 3, 1);

	grid->addWidget(param3Label, 4, 0);
	grid->addWidget(param3Edit, 4, 1);

	grid->addWidget(fpsLabel_, 5, 0);
	grid->addWidget(fpsLabelValue_, 5, 1);
	setLayout(grid);
}
