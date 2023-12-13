#ifndef NFB_FACES_H
#define NFB_FACES_H

#include "../TBV/TBVPluginInterface.h"
#include "../Trigger/TriggerThread.h"
#include "FeedbackPresenter.h"
#include "TimerThread.h"
#include "RTLogTable.h"



class NFB_Faces : public QObject, public TBVPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(TBVPluginInterface)

public:
	
	PLUGIN_ACCESS NFB_Faces(QObject *parent = 0);
	PLUGIN_ACCESS ~NFB_Faces();

	PLUGIN_ACCESS bool initPlugin();
	PLUGIN_ACCESS bool executePreRun();
	PLUGIN_ACCESS bool executePreStep();
	PLUGIN_ACCESS bool executePostStep();
	PLUGIN_ACCESS bool executePostRun();

private:

	float						calcBaseline();

	FeedbackPresenter			*myFeedbackPresenter;
	TriggerThread				*myTriggerThread;
	TimerThread					*myTimerThread;
	RTLogTable					*myRTLogTable;

	int							iMoveBaseWinSize;
	float						fMaxPercChange, fBaseline;
	char						cLogString[601], cPluginsPath[101], cProjectName[101];

	QElapsedTimer				qetTimer;
	uint						uiTriggerCounter;

	QVector<QVariant>			dataValuesTMP;
	QVector<int>				qviBaseline;
	QList<QVector<QVariant>>	dataValues;

signals:

	void	ImageReceived();
	void	ImageProcessed();
	void	StopPresentation();

private slots:

	void	incPercMax();
	void	decPercMax();

};

// To avoid C++ name mangling problems, we need to bootstrap access of plugin via C functions
extern "C" {
	PLUGIN_ACCESS TBVPluginInterface *createPlugin();
	PLUGIN_ACCESS const char *getPluginName();
	PLUGIN_ACCESS const char *getPluginDescription();
	PLUGIN_ACCESS const char *getAuthor();
	PLUGIN_ACCESS const char *getHelpFile();
	PLUGIN_ACCESS const char *getPluginVersion();
}

#endif
