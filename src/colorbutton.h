#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H

#include <QPushButton>


/**
    Kontrolka do wyboru koloru
**/
class ColorButton : public QPushButton
{
	Q_OBJECT

	QColor currentColor;

public:
	ColorButton(QWidget *parent = 0);
	const QColor &color() const;
	bool setColor(const QColor &color);

private slots:
	void onClick();

signals:
	void changed(const QColor &color);

};

#endif // COLOR_BUTTON_H
