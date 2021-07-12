//
// Created by lily on 7/12/21.
//

#ifndef EAGLE_ABOUTUI_H
#define EAGLE_ABOUTUI_H

#include <QDialog>

#include "ui_AboutUi.h"

namespace eagle::ui {

	/**
 	 * Simple modal about UI.
 	 */
	struct AboutUi : public QDialog {
		Q_OBJECT
	   public:
		AboutUi(QWidget* parent);

	   private:
		Ui::AboutUi ui;
	};

} // namespace eagle::ui

#endif //EAGLE_ABOUTUI_H
