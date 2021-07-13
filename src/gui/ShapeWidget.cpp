#include "ShapeWidget.h"
#include <QPainter>
#include <QLayout>

namespace ssxtools::ui {

	void ShapeWidget::PaintShape(shps::Image& shape, bool ssxHack) {
		shps::ShpsConverter writer;

		// Build the raw RGBA8888 image buffer.
		if(!writer.BuildImageBuffer(rawBuffer, shape, ssxHack))
			return;

		// Feed it into the QImage constructor designed for dealing with raw data
		// (since ShpsConverter::BuildImageBuffer creates *standardized* RGBA8888 data).
		qtImage = QImage(rawBuffer.data(), shape.width, shape.height, QImage::Format_RGBA8888, nullptr, nullptr);

		// Resize and queue a update of the widget for the shape's size.
		QWidget::resize(shape.width, shape.height);
		QWidget::update();
	}

	void ShapeWidget::paintEvent(QPaintEvent* ev) {
		QWidget::paintEvent(ev);

		QPainter p(this);
		p.drawPixmap(0, 0, QPixmap::fromImage(qtImage));
	}

} // namespace ssxtools::ui