#include "FeedbackPresenter.h"

FeedbackPresenter::FeedbackPresenter(QElapsedTimer &qetTimer)
{
	setWindowTitle(tr("FeedbackPresenter"));
	setWindowFlags( windowFlags() ^ Qt::WindowStaysOnTopHint );
	setFixedSize( QSize(1024,768) );
	setStyleSheet("color: white; background-color: black");

	myPixLabel = new QLabel;
	myPixLabel->setAlignment(Qt::AlignCenter);   
 	myPixLabel->setFont( QFont("Helvetica [Cronyx]", 70) );

	myTextLabel = new QLabel;
	myTextLabel->setAlignment(Qt::AlignBottom|Qt::AlignRight);

	QVBoxLayout *myLayout = new QVBoxLayout();
	//myLayout->addSpacing(250);
	myLayout->addStretch();
	myLayout->addWidget(myPixLabel);
	myLayout->addStretch();
	//myLayout->addWidget(myTextLabel);
    setLayout(myLayout);

	qTimer = &qetTimer;

	FixImg.load(":/Fixation");
	Neutral.load(":/Neutral");
	N1.load(":/N1");
	N2.load(":/N2");
	N3.load(":/N3");
	N4.load(":/N4");
	N5.load(":/N5");
	N6.load(":/N6");
	N7.load(":/N7");
	N8.load(":/N8");
	P1.load(":/P1");
	P2.load(":/P2");
	P3.load(":/P3");
	P4.load(":/P4");
	P5.load(":/P5");
	P6.load(":/P6");
	P7.load(":/P7");
	P8.load(":/P8");
}

FeedbackPresenter::~FeedbackPresenter()
{
}

void FeedbackPresenter::showImage(int value)
{
	switch( value )
	{
		case 0:		myPixLabel->setPixmap(QPixmap::fromImage(N8)); emit LogCondition(value); break;
		case 1:		myPixLabel->setPixmap(QPixmap::fromImage(N7)); emit LogCondition(value); break;
		case 2:		myPixLabel->setPixmap(QPixmap::fromImage(N6)); emit LogCondition(value); break;
		case 3:		myPixLabel->setPixmap(QPixmap::fromImage(N5)); emit LogCondition(value); break;
		case 4:		myPixLabel->setPixmap(QPixmap::fromImage(N4)); emit LogCondition(value); break;
		case 5:		myPixLabel->setPixmap(QPixmap::fromImage(N3)); emit LogCondition(value); break;
		case 6:		myPixLabel->setPixmap(QPixmap::fromImage(N2)); emit LogCondition(value); break;
		case 7:		myPixLabel->setPixmap(QPixmap::fromImage(N1)); emit LogCondition(value); break;
		case 8:		myPixLabel->setPixmap(QPixmap::fromImage(Neutral)); emit LogCondition(value); break;
		case 9:		myPixLabel->setPixmap(QPixmap::fromImage(P1)); emit LogCondition(value); break;
		case 10:	myPixLabel->setPixmap(QPixmap::fromImage(P2)); emit LogCondition(value); break;
		case 11:	myPixLabel->setPixmap(QPixmap::fromImage(P3)); emit LogCondition(value); break;
		case 12:	myPixLabel->setPixmap(QPixmap::fromImage(P4)); emit LogCondition(value); break;
		case 13:	myPixLabel->setPixmap(QPixmap::fromImage(P5)); emit LogCondition(value); break;
		case 14:	myPixLabel->setPixmap(QPixmap::fromImage(P6)); emit LogCondition(value); break;
		case 15:	myPixLabel->setPixmap(QPixmap::fromImage(P7)); emit LogCondition(value); break;
		case 16:	myPixLabel->setPixmap(QPixmap::fromImage(P8)); emit LogCondition(value); break;
		case 99:	myPixLabel->setPixmap(QPixmap::fromImage(FixImg)); emit LogBaseline(); break;
	}
}

void FeedbackPresenter::updateTextLabel(QString qsText)
{
	myTextLabel->setText( qsText );
}

void FeedbackPresenter::StartBaseline()
{
	showImage(99);
}

void FeedbackPresenter::StartCondition()
{
	showImage(0);
}

void FeedbackPresenter::StopPresentation()
{
	myPixLabel->clear();
}

void FeedbackPresenter::mouseDoubleClickEvent(QMouseEvent *e)
{
	if( isFullScreen() )
		setWindowState( Qt::WindowMaximized );
	else
		setWindowState( Qt::WindowFullScreen );
}