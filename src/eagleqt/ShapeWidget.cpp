#include "ShapeWidget.h"
#include <QPainter>
#include <QLayout>

namespace eagle::ui {

	void ShapeWidget::PaintShape(core::shps::Image& shape) {
		core::ShpsConverter writer;

		// Build the raw RGBA8888 image buffer.
		if(!writer.BuildImageBuffer(rawBuffer, shape))
			return;

		// Feed it into the QImage constructor designed for this.
		qtImage = QImage(rawBuffer.data(), shape.width, shape.height, QImage::Format_RGBA8888, nullptr, nullptr);

		/*
		auto* ptr = reinterpret_cast<eagle::core::shps::Bgra8888*>(rawBuffer.data());
		qtImage = QImage(shape.width, shape.height, QImage::Format_RGBA8888);

		for(int x = 0; x < shape.width; ++x) {
			for(int y = 0; y < shape.height; ++y) {
				const auto& shapecolor = ptr[y * shape.width + x];
				qtImage.setPixel(x, y, qRgba(shapecolor.b, shapecolor.g, shapecolor.r, shapecolor.a));
			}
		}
		*/

		// Resize and queue a update of the widget.
		QWidget::resize(shape.width, shape.height);
		QWidget::update();
	}

	void ShapeWidget::paintEvent(QPaintEvent* ev) {
		QWidget::paintEvent(ev);

		QPainter p(this);
		p.drawPixmap(0, 0, QPixmap::fromImage(qtImage));
	}

} // namespace eagle::ui