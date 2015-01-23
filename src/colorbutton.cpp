/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QColorDialog>
#include <QtGui/QPainter>

#include "colorbutton.h"

ColorButton::ColorButton(QWidget *parent)
	: QPushButton(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void ColorButton::onClick()
{
        if (setColor(QColorDialog::getColor(currentColor, this, "Wybór koloru")))
		emit changed(currentColor);
}

const QColor & ColorButton::color() const
{
	return currentColor;
}

bool ColorButton::setColor(const QColor &color)
{
	if (!color.isValid())
		return false;

	currentColor = color;
        QPixmap pm(40, 12);
	pm.fill(color);
        QPainter p(&pm);
        p.setPen(QPen(Qt::white));
        p.drawRect(0, 0, 39, 11);
        setIconSize(QSize(40, 12));
        setIcon(QIcon(pm));

	return true;
}
