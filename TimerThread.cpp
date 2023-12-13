#include "TimerThread.h"



TimerThread::TimerThread(QElapsedTimer &qetTimer, uint &uiTriggerCounter, QString qsPluginsPath, QString qsProjectName)
{
	qsPath				= qsPluginsPath;
	qsProject			= qsProjectName;
	bStarted			= false;
	bBaseline			= false;
	iTriggerCounter		= &uiTriggerCounter;
	qTimer				= &qetTimer;
	qtTriggerSimTimer	= new QTimer(this);
	connect(qtTriggerSimTimer, SIGNAL( timeout() ), this, SLOT( TriggerReceived() ));
}

TimerThread::~TimerThread()
{
}

void TimerThread::run()
{
}

void TimerThread::TriggerSimSwitch()
{
	if( !bStarted )
	{
		qtTriggerSimTimer->start( 2000-3 );
		TriggerReceived();
	}
	else
	{
		if( qtTriggerSimTimer->isActive() )
		{
			qtTriggerSimTimer->stop();
		}
	}
}

void TimerThread::TriggerReceived()
{
	*iTriggerCounter = *iTriggerCounter + 1;
	if( !bStarted )
	{
		qTimer->start();
		bStarted = true;
	}
	updatePresentation();
	if( qtTriggerSimTimer->isActive() )
	{
		emit SimulatedTrigger();
	}
}

void TimerThread::updatePresentation()
{
	switch( *iTriggerCounter )
	{
		case 1:	case 26: case 51: case 76: case 101: case 126: case 151: case 176: case 201: case 226: case 251:  case 276: case 301:
			emit StartBaseline();
			bBaseline = true;
			break;
		case 11: case 36: case 61: case 86: case 111: case 136: case 161: case 186: case 211: case 236: case 261: case 286:
			emit StartCondition();
			bBaseline = false;
			break;
		case 311:
			if( qtTriggerSimTimer->isActive() )
				qtTriggerSimTimer->stop();
			break;
	}
}


void TimerThread::LogImgReceived()
{
	qlImgReceived.append( qTimer->elapsed() );
}

void TimerThread::LogImgProcessed()
{
	qlImgProcessed.append( qTimer->elapsed() );
}

void TimerThread::LogBaseline()
{
	qlBase.append( qTimer->elapsed() );
}

void TimerThread::LogCondition(int cond)
{
	switch( cond )
	{
		case 0:		qlCond00.append( qTimer->elapsed() ); break;
		case 1:		qlCond01.append( qTimer->elapsed() ); break;
		case 2:		qlCond02.append( qTimer->elapsed() ); break;
		case 3:		qlCond03.append( qTimer->elapsed() ); break;
		case 4:		qlCond04.append( qTimer->elapsed() ); break;
		case 5:		qlCond05.append( qTimer->elapsed() ); break;
		case 6:		qlCond06.append( qTimer->elapsed() ); break;
		case 7:		qlCond07.append( qTimer->elapsed() ); break;
		case 8:		qlCond08.append( qTimer->elapsed() ); break;
		case 9:		qlCond09.append( qTimer->elapsed() ); break;
		case 10:	qlCond10.append( qTimer->elapsed() ); break;
	}

}

QString TimerThread::WriteLogFile(QList<QVector<QVariant>> dataValues)
{
	QString qsFilename = qsPath + "/" + QDateTime::currentDateTime().toString("yyyyMMddhhmm") + "_" + qsProject + ".txt";
	QFile qfOut(qsFilename);
	qfOut.open(QIODevice::WriteOnly);
	if( qfOut.isOpen() )
	{
		QTextStream qtStream(&qfOut);

		qtStream << "ImgReceived: " << "\t";
		for(int i=0; i<qlImgReceived.size(); i++)
		{
			qtStream << qlImgReceived[i] << "\t";
		}
		qtStream << endl;

		qtStream << "ImgProcessed: ";
		for(int i=0; i<qlImgProcessed.size(); i++)
		{
			qtStream << qlImgProcessed[i] << "\t";
		}
		qtStream << endl;

		qtStream << "Baseline: " << "\t\t";
		for(int i=0; i<qlBase.size(); i++)
		{
			qtStream << qlBase[i] << "\t";
		}
		qtStream << endl;

		qtStream << "Condition00: " << "\t";
		for(int i=0; i<qlCond00.size(); i++)
		{
			qtStream << qlCond00[i] << "\t";
		}
		qtStream << endl;

		qtStream << "Condition01: " << "\t";
		for(int i=0; i<qlCond01.size(); i++)
		{
			qtStream << qlCond01[i] << "\t";
		}
		qtStream << endl;

		qtStream << "Condition02: " << "\t";
		for(int i=0; i<qlCond02.size(); i++)
		{
			qtStream << qlCond02[i] << "\t";
		}
		qtStream << endl;

		qtStream << "Condition03: " << "\t";
		for(int i=0; i<qlCond03.size(); i++)
		{
			qtStream << qlCond03[i] << "\t";
		}
		qtStream << endl;

		qtStream << "Condition04: " << "\t";
		for(int i=0; i<qlCond04.size(); i++)
		{
			qtStream << qlCond04[i] << "\t";
		}
		qtStream << endl;

		qtStream << "Condition05: " << "\t";
		for(int i=0; i<qlCond05.size(); i++)
		{
			qtStream << qlCond05[i] << "\t";
		}
		qtStream << endl;

		qtStream << "Condition06: " << "\t";
		for(int i=0; i<qlCond06.size(); i++)
		{
			qtStream << qlCond06[i] << "\t";
		}
		qtStream << endl;

		qtStream << "Condition07: " << "\t";
		for(int i=0; i<qlCond07.size(); i++)
		{
			qtStream << qlCond07[i] << "\t";
		}
		qtStream << endl;

		qtStream << "Condition08: " << "\t";
		for(int i=0; i<qlCond08.size(); i++)
		{
			qtStream << qlCond08[i] << "\t";
		}
		qtStream << endl;

		qtStream << "Condition09: " << "\t";
		for(int i=0; i<qlCond09.size(); i++)
		{
			qtStream << qlCond09[i] << "\t";
		}
		qtStream << endl;

		qtStream << "Condition10: " << "\t";
		for(int i=0; i<qlCond10.size(); i++)
		{
			qtStream << qlCond10[i] << "\t";
		}
		qtStream << endl;

		qtStream << endl;
		for(int i=0; i<dataValues.size(); i++)
		{
			qtStream << i+1 << "\t";
			for(int j=0; j<dataValues[i].size(); j++)
			{
				qtStream << dataValues[i][j].toString() << "\t";
			}
			qtStream << endl;
		}

 		qfOut.close();
	}
	return qsFilename;
}

bool TimerThread::BaselinePeriod()
{
	return bBaseline;
}

bool TimerThread::isStarted()
{
	return bStarted;
}


