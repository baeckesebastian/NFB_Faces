#include "NFB_Faces.h"

const float PlugInVersion = 0.7;



NFB_Faces::NFB_Faces(QObject *parent)
{	
}

NFB_Faces::~NFB_Faces()
{
	myTriggerThread->terminate();
	myTimerThread->terminate();
	myFeedbackPresenter->close();
	myRTLogTable->close();
}

bool NFB_Faces::initPlugin() 
{
	return InitTBVAccess();
}

bool NFB_Faces::executePreRun()
{
	uiTriggerCounter = 0;

	/* RTLogTable */
	//int iTR = tGetIntegerInput("TR in ms: ", 2000, 1, 5000);
	myRTLogTable = new RTLogTable(qetTimer, 2000, uiTriggerCounter);
    myRTLogTable->show();

	/* size for sliding window */
	iMoveBaseWinSize = tGetIntegerInput("Window size for moving window?", 10, 1, 100);
	sprintf_s(cLogString, "<b>MovingWindowBaseline (Size:%i)</b>", iMoveBaseWinSize);
	tLogText(cLogString);
	
	/* maximum percentage signal increase */
	fMaxPercChange = tGetFloatInput("Maximum percentage signal increase?", 2.5, 0.5, 10);
	sprintf_s(cLogString, "<b>fMaxPercChange:</b> %2.2f%%", fMaxPercChange);
	tLogText(cLogString);

	/* TimerThread */
	tGetPluginsPath(cPluginsPath);
	tGetProjectName(cProjectName);
	myTimerThread = new TimerThread(qetTimer, uiTriggerCounter, QString(cPluginsPath), QString(cProjectName));
	myTimerThread->start();

	/* TriggerThread */
	//char cTriggerPort[601];
	//if(tGetTextInput("Trigger Memory Address: ", "C011", cTriggerPort))		// 7T: EC00????
	char cTriggerPort[] = "C011";
	{
		QString qsTriggerPort = cTriggerPort;
		myTriggerThread = new TriggerThread( qsTriggerPort.toUInt(0, 16) );
		myTriggerThread->start();
		if( myTriggerThread->isRunning() )
			tLogText("TriggerThread is running.");
	}

	/* FeedbackPresenter */
	myFeedbackPresenter = new FeedbackPresenter(qetTimer);
    myFeedbackPresenter->show();

	/* TriggerSimulation */
	QShortcut *qHiddenShortcut1 = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_T), myFeedbackPresenter);
	tLogText("  Strg+T: \tTrigger-Simulation Switch");
	
	/* Increase/Decrease maximum percentage signal change */
	QShortcut *qHiddenShortcut2 = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_Plus), myFeedbackPresenter);
	tLogText("  Strg++: \tIncrease maximum percentage signal change (+0.1%)");
	QShortcut *qHiddenShortcut3 = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_Minus), myFeedbackPresenter);
	tLogText("  Strg+-: \tDecrease maximum percentage signal change (-0.1%)");

	/* CONNECTIONS */
	connect( myTriggerThread,		SIGNAL( TriggerReceived() ),	myTimerThread,			SLOT( TriggerReceived() ));
	connect( myTriggerThread,		SIGNAL( TriggerReceived() ),	myRTLogTable,			SLOT( TriggerReceivedLog() ));
	connect( this,					SIGNAL( ImageReceived() ),		myTimerThread,			SLOT( LogImgReceived() ));
	connect( this,					SIGNAL( ImageProcessed() ),		myTimerThread,			SLOT( LogImgProcessed() ));
	connect( this,					SIGNAL( ImageReceived() ),		myRTLogTable,			SLOT( ImageReceivedLog() ));
	connect( this,					SIGNAL( ImageProcessed() ),		myRTLogTable,			SLOT( ImageProcessedLog() ));
	connect( this,					SIGNAL( StopPresentation() ),	myFeedbackPresenter,	SLOT( StopPresentation() ));
	connect( qHiddenShortcut1,		SIGNAL( activated() ),			myTimerThread,			SLOT( TriggerSimSwitch() ));
	connect( qHiddenShortcut2,		SIGNAL( activated() ),			this,					SLOT( incPercMax() ));
	connect( qHiddenShortcut3,		SIGNAL( activated() ),			this,					SLOT( decPercMax() ));
	connect( myTimerThread,			SIGNAL( StartBaseline() ),		myFeedbackPresenter,	SLOT( StartBaseline() ));
	connect( myTimerThread,			SIGNAL( StartCondition() ),		myFeedbackPresenter,	SLOT( StartCondition() ));
	connect( myTimerThread,			SIGNAL( SimulatedTrigger() ),	myRTLogTable,			SLOT( TriggerReceivedLog() ));
	connect( myFeedbackPresenter,	SIGNAL( LogBaseline() ),		myTimerThread,			SLOT( LogBaseline() ));
	connect( myFeedbackPresenter,	SIGNAL( LogCondition(int) ),	myTimerThread,			SLOT( LogCondition(int) ));

	tLogText("  DoubleClick: \tFull-Screen Switch");
	tLogText("<br>");
	return true;
}

bool NFB_Faces::executePreStep()
{
	emit ImageReceived();
	dataValuesTMP.clear();
	return true;
}

bool NFB_Faces::executePostStep()
{
	emit ImageProcessed();

	int iCurrentTR		= tGetCurrentTimePoint(); // access time point just finished processing in TBV: 1-based, for access of 0-based data, subtract 1
	int iExpectedNrOfTR = tGetExpectedNrOfTimePoints();
	int iNrOfROIs		= tGetNrOfROIs(); 

	dataValuesTMP.append( iNrOfROIs ); // [0]

	float fValueOfDesignMatrix = tGetValueOfDesignMatrix(0, iCurrentTR-1);

	if(fValueOfDesignMatrix < 0.1)
	{
		qviBaseline.append( iCurrentTR-1 );
	}
	
	sprintf_s(cLogString, "<b>%04i</b>/%04i (NrOfROIs=%02i): ", iCurrentTR, iExpectedNrOfTR, iNrOfROIs);
	
	if(iNrOfROIs > 0)
	{
		/*for(int i=0; i<iNrOfROIs; i++)
		{*/
		int i = 0;
			int iNrOfVoxelsOfROI = tGetNrOfVoxelsOfROI(i);
			float fMeanOfROI = tGetMeanOfROI(i);
			sprintf_s(cLogString, "%s [%04i: %4.3f]", cLogString, iNrOfVoxelsOfROI, fMeanOfROI);
			dataValuesTMP.append(iNrOfVoxelsOfROI); //[1]
			dataValuesTMP.append(fMeanOfROI);	//[2]
		/*}*/

		fBaseline = calcBaseline();
		dataValuesTMP.append(fBaseline);	//[3]

		char cTMP[601];
		sprintf_s(cTMP, "Baseline: %3.3f   ROI: %3.3f   MAX: %2.2f%%", fBaseline, dataValuesTMP[2].toFloat(), fMaxPercChange);

		if(fValueOfDesignMatrix < 0.1)
		{
			sprintf_s(cLogString, "%s <b>%4.3f</b>", cLogString, fBaseline);
		}
		else
		{
			float fPercSignChange = ((dataValuesTMP[2].toFloat() * 100) / fBaseline) - 100;
			if( myTimerThread->isStarted() && !myTimerThread->BaselinePeriod() )
			{
				int iTMP = qRound(16*(fPercSignChange/fMaxPercChange));
				if( iTMP < 0 )	iTMP = 0;
				if( iTMP > 16 )	iTMP = 16;
				myFeedbackPresenter->showImage( iTMP );
				sprintf_s(cTMP, "Baseline: %3.3f   ROI: %3.3f   ->   %2.2f%% (S%02i)   MAX: %2.2f%%", fBaseline, dataValuesTMP[2].toFloat(), fPercSignChange, iTMP, fMaxPercChange);
				sprintf_s(cLogString, "%s <b><font color='red'>%2.2f%% (S%02i)</font></b>", cLogString, fPercSignChange, iTMP);
				dataValuesTMP.append(fPercSignChange);	//[4]
				dataValuesTMP.append(fMaxPercChange);	//[5]
				dataValuesTMP.append(iTMP);	//[6]
			}
		}

		myFeedbackPresenter->updateTextLabel( cTMP );
	}
	tLogText(cLogString);

	dataValues.append(dataValuesTMP);

	return true;
}

bool NFB_Faces::executePostRun()
{
	emit StopPresentation();

	tLogText( "<br><b>LOGFILE:</b>" );
	QString qsFilename = myTimerThread->WriteLogFile(dataValues);
	tLogText( qsFilename.toStdString().c_str() );
	tLogText("<br>Plugin completed processing.");

	myRTLogTable->finalize();
	myTimerThread->terminate();
	myTriggerThread->terminate();

	return true;
}

float NFB_Faces::calcBaseline()
{
	int iTimepoints		= iMoveBaseWinSize;
	int iNrVoxels		= tGetNrOfVoxelsOfROI(0);
	if( iTimepoints > qviBaseline.size() )
		iTimepoints		= qviBaseline.size();
	float fMeanSignal	= 0.0;

	for(int vx=0; vx<iNrVoxels; vx++)
	{
		int vx_x, vx_y, vx_z;
		tGetCoordsOfVoxelOfROI(0, vx, vx_x, vx_y, vx_z);
		
		for(int t=qviBaseline.size()-1; t>=qviBaseline.size()-iTimepoints; t--)
		{
			fMeanSignal += tGetValueOfVoxelAtTime(vx_x, vx_y, vx_z, qviBaseline[t]);
		}
	}
	return fMeanSignal / (iTimepoints * iNrVoxels);
}

void NFB_Faces::incPercMax()
{
	fMaxPercChange += 0.1;
}

void NFB_Faces::decPercMax()
{
	fMaxPercChange -= 0.1;
}



PLUGIN_ACCESS TBVPluginInterface *createPlugin() 
{
	return new NFB_Faces();
}

PLUGIN_ACCESS const char *getPluginName() 
{
	char *PluginName = new char[100];
	sprintf_s(PluginName, 100, "NEUROFEEDBACK [Faces] v%1.1f", PlugInVersion);
	return PluginName;
}

static const char *PluginDescription = "This simple plugin shows ...";
PLUGIN_ACCESS const char *getPluginDescription() 
{
	return PluginDescription;
}

PLUGIN_ACCESS const char *getAuthor()
{
	return "<i>Sebastian Baecke</i>, Otto-von-Guericke-University Magdeburg";
}

PLUGIN_ACCESS const char *getHelpFile()
{
	return ""; // "NFB_FacesHelp/index.html";
}

PLUGIN_ACCESS const char *getPluginVersion()
{
	char *PluginVersionString = new char[6];
	sprintf_s(PluginVersionString, 6, "%1.1f", PlugInVersion);
	return PluginVersionString;
}
