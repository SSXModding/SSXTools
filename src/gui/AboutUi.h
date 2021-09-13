//
// Created by lily on 7/12/21.
//

#ifndef SSXTOOLS_ABOUTUI_H
#define SSXTOOLS_ABOUTUI_H

#include <QDialog>

#include "ui_AboutUi.h"

namespace ssxtools::ui {

	/**
 	 * Simple little modal about UI.
 	 */
	struct AboutUi : public QDialog {
		Q_OBJECT
	   public:
		AboutUi(QWidget* parent);

	   private:
		Ui::AboutUi ui;
	};

} // namespace ssxtools::ui

#endif //SSXTOOLS_ABOUTUI_H
