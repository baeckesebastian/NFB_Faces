#ifndef FEEDBACKPRESENTER_H
#define FEEDBACKPRESENTER_H

#include <QtGui>



class FeedbackPresenter : public QWidget
{
	Q_OBJECT

public:

	FeedbackPresenter(QElapsedTimer &qetTimer);
	~FeedbackPresenter();
	void showImage(int value);
	void updateTextLabel(QString qsText);
	
private:

	void			mouseDoubleClickEvent(QMouseEvent *e);

	QImage			FixImg, Neutral, N1, N2, N3, N4, N5, N6, N7, N8, P1, P2, P3, P4, P5, P6, P7, P8;
	QLabel			*myPixLabel, *myTextLabel;
	QElapsedTimer	*qTimer;

signals:

	void LogBaseline();
	void LogCondition(int);

private slots:

	void StartBaseline();
	void StartCondition();
	void StopPresentation();

};

#endif