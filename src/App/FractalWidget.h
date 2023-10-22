#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QWidget>

class FractalWidget : public QWidget {
public:
	FractalWidget(QWidget * parent = nullptr);
	QLabel * fpsLabel_;
	QLabel * fpsLabelValue_;
	QLabel * param1Label;
	QLabel * param2Label;
	QLabel * param3Label;
	QLabel * iterationsLabel_;
	
	QSlider * param1Edit;
	QSlider * param2Edit;
	QSlider * param3Edit;
	QSlider * iterationsEdit;
};
