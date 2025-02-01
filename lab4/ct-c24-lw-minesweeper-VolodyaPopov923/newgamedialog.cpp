#include "NewGameDialog.h"

NewGameDialog::NewGameDialog(QWidget *parent) : QDialog(parent)
{
	QGridLayout *layout = new QGridLayout(this);
	n2 = new QLabel(tr("Rows:"));
	layout->addWidget(n2, 0, 0);
	rowsSpinBox = new QSpinBox(this);
	rowsSpinBox->setRange(2, 100);
	rowsSpinBox->setValue(10);
	layout->addWidget(rowsSpinBox, 0, 1);
	n1 = new QLabel(tr("Columns:"));
	layout->addWidget(n1, 1, 0);
	colsSpinBox = new QSpinBox(this);
	colsSpinBox->setRange(2, 100);
	colsSpinBox->setValue(10);
	layout->addWidget(colsSpinBox, 1, 1);
	m = new QLabel(tr("Mines:"));
	layout->addWidget(m, 2, 0);

	minesSpinBox = new QSpinBox(this);
	minesSpinBox->setRange(1, 10000);
	minesSpinBox->setValue(10);
	layout->addWidget(minesSpinBox, 2, 1);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	layout->addWidget(buttonBox, 3, 0, 1, 2);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(
		rowsSpinBox,
		QOverload< int >::of(&QSpinBox::valueChanged),
		this,
		[this, buttonBox]()
		{
			if (minesSpinBox->value() >= rowsSpinBox->value() * colsSpinBox->value())
			{
				buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
			}
			else
			{
				buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
			}
		});

	connect(
		colsSpinBox,
		QOverload< int >::of(&QSpinBox::valueChanged),
		this,
		[this, buttonBox]()
		{
			if (minesSpinBox->value() >= rowsSpinBox->value() * colsSpinBox->value())
			{
				buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
			}
			else
			{
				buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
			}
		});

	connect(
		minesSpinBox,
		QOverload< int >::of(&QSpinBox::valueChanged),
		this,
		[this, buttonBox]()
		{
			if (minesSpinBox->value() >= rowsSpinBox->value() * colsSpinBox->value())
			{
				buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
			}
			else
			{
				buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
			}
		});
}
void NewGameDialog::setT(bool mode)
{
	if (mode)
	{
		m->setText("Мины:");
		n1->setText("Столбцы:");
		n2->setText("Строки:");
	}
	else
	{
		m->setText("Mines:");
		n1->setText("Columns");
		n2->setText("Rows");
	}
}
uint16_t NewGameDialog::rows() const
{
	return rowsSpinBox->value();
}
uint16_t NewGameDialog::cols() const
{
	return colsSpinBox->value();
}
uint16_t NewGameDialog::mines() const
{
	return minesSpinBox->value();
}
