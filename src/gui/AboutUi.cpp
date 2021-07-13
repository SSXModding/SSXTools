//
// Created by lily on 7/12/21.
//

#include "AboutUi.h"

namespace ssxtools::ui {

	AboutUi::AboutUi(QWidget* parent)
		: QDialog(parent) {
		ui.setupUi(this);

		connect(ui.ok, &QPushButton::pressed, [this]() {
			this->close();
		});
	}

} // namespace ssxtools::ui