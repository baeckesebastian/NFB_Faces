#include "RTLogTable.h"
#include <QApplication>

RTLogTable::RTLogTable(QElapsedTimer &qetTimer, int TR, uint &uiTriggerCounter)
{
	setWindowTitle("real-time LogWindow");
	setFixedSize(348, 730);

	tableWidget = new QTableWidget(0, 3, this);	// neue Tabelle 0 Zeilen x 2 Spalten erzeugen
	setCentralWidget(tableWidget);

	QStringList horizontHeader = (QStringList() << "Trigger" << "TransferTime" << "ProcessingTime");
	tableWidget->setHorizontalHeaderLabels ( horizontHeader );

	iTriggerCounter		= &uiTriggerCounter;
	uiImgCounter		= 0;
	uiProcCounter		= 0;

	qtTriggerEmulationTimer= new QTimer(this);
	connect(qtTriggerEmulationTimer, SIGNAL(timeout()), this, SLOT( TriggerReceivedLog() ));

	iTR = TR;
	qTimer = &qetTimer;
}

void RTLogTable::TriggerReceivedLog()
{
	tableWidget->setRowCount( *iTriggerCounter );

	verticHeader.append(tr("%1").arg(*iTriggerCounter, 3, 10, QChar('0')));
	tableWidget->setVerticalHeaderLabels (verticHeader);

	QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg( qTimer->elapsed() ));
	newItem->setFlags(Qt::ItemIsSelectable);
	tableWidget->setItem( *iTriggerCounter-1, 0, newItem );
	tableWidget->scrollToBottom();
}

void RTLogTable::ImageReceivedLog()
{
	uiImgCounter++;

	if( uiImgCounter <= *iTriggerCounter )
	{
		qListTransferTime << qTimer->elapsed() - tableWidget->item(uiImgCounter-1, 0)->text().toUInt() - iTR;
		QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg( qListTransferTime.last() ));
		newItem->setFlags(Qt::ItemIsSelectable);
		tableWidget->setItem( uiImgCounter-1, 1, newItem );
	}
}	

void RTLogTable::ImageProcessedLog()
{
	uiProcCounter++;

	if( uiProcCounter <= *iTriggerCounter )
	{
		qListProcessingTime << qTimer->elapsed() - tableWidget->item(uiProcCounter-1, 1)->text().toUInt() - tableWidget->item(uiProcCounter-1, 0)->text().toUInt() - iTR;
		QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg( qListProcessingTime.last() ));
		newItem->setFlags(Qt::ItemIsSelectable);
		tableWidget->setItem( uiProcCounter-1, 2, newItem );
	}
}

void RTLogTable::finalize()
{
	tableWidget->setRowCount( uiImgCounter+4 );
	
	verticHeader.clear();
	for(int i=1; i<=uiImgCounter; i++)
		verticHeader.append(tr("%1").arg(i, 3, 10, QChar('0')));
	verticHeader << " " << " " << " " << " ";
	tableWidget->setVerticalHeaderLabels (verticHeader);

	QTableWidgetItem *Item01 = new QTableWidgetItem(tr("MEAN: "));
	Item01->setBackground(QBrush(Qt::lightGray));
	Item01->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
	tableWidget->setItem( uiImgCounter, 0, Item01 );

	QTableWidgetItem *Item02 = new QTableWidgetItem(tr("SD: "));
	Item02->setBackground(QBrush(Qt::lightGray));
	Item02->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
	tableWidget->setItem( uiImgCounter+1, 0, Item02 );

	QTableWidgetItem *Item03 = new QTableWidgetItem(tr("min: "));
	Item03->setBackground(QBrush(Qt::lightGray));
	Item03->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
	tableWidget->setItem( uiImgCounter+2, 0, Item03 );

	QTableWidgetItem *Item04 = new QTableWidgetItem(tr("max: "));
	Item04->setBackground(QBrush(Qt::lightGray));
	Item04->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
	tableWidget->setItem( uiImgCounter+3, 0, Item04 );

	qListTransferTime.removeFirst();
	qSort(qListTransferTime.begin(), qListTransferTime.end());
	
	float fMean1 = 0;
	for (int i=0; i<qListTransferTime.size(); i++)
	{
		fMean1 += qListTransferTime[i];
	}
	fMean1 /= qListTransferTime.size();

	float fVarianz1 = 0;
	for (int i=0; i<qListTransferTime.size(); i++)
	{
		fVarianz1 += ((qListTransferTime[i]-fMean1)*(qListTransferTime[i]-fMean1));
	}
	fVarianz1 /= qListTransferTime.size();

	QTableWidgetItem *Item11 = new QTableWidgetItem(tr("%1ms").arg( fMean1, 0, 'f', 2 ));
	Item11->setBackground(QBrush(Qt::lightGray));
	tableWidget->setItem( uiImgCounter, 1, Item11 );
	QTableWidgetItem *Item12 = new QTableWidgetItem(tr("%1ms").arg( qSqrt(fVarianz1), 0, 'f', 2 ));
	Item12->setBackground(QBrush(Qt::lightGray));
	tableWidget->setItem( uiImgCounter+1, 1, Item12 );
	QTableWidgetItem *Item13 = new QTableWidgetItem(tr("%1ms").arg( qListTransferTime[0] ));
	Item13->setBackground(QBrush(Qt::lightGray));
	tableWidget->setItem( uiImgCounter+2, 1, Item13 );
	QTableWidgetItem *Item14 = new QTableWidgetItem(tr("%1ms").arg( qListTransferTime[uiImgCounter-2] ));
	Item14->setBackground(QBrush(Qt::lightGray));
	tableWidget->setItem( uiImgCounter+3, 1, Item14 );

	qListProcessingTime.removeFirst();
	qSort(qListProcessingTime.begin(), qListProcessingTime.end());
	float fMean2 = 0;
	for (int i=0; i<qListProcessingTime.size(); i++)
	{
		fMean2 += qListProcessingTime[i];
	}
	fMean2 /= qListProcessingTime.size();

	float fVarianz2 = 0;
	for (int i=0; i<qListProcessingTime.size(); i++)
	{
		fVarianz2 += ((qListProcessingTime[i]-fMean2)*(qListProcessingTime[i]-fMean2));
	}
	fVarianz2 /= qListProcessingTime.size();

	QTableWidgetItem *Item21 = new QTableWidgetItem(tr("%1ms").arg( fMean2, 0, 'f', 2 ));
	Item21->setBackground(QBrush(Qt::lightGray));
	tableWidget->setItem( uiImgCounter, 2, Item21 );
	QTableWidgetItem *Item22 = new QTableWidgetItem(tr("%1ms").arg( qSqrt(fVarianz2), 0, 'f', 2  ));
	Item22->setBackground(QBrush(Qt::lightGray));
	tableWidget->setItem( uiImgCounter+1, 2, Item22 );
	QTableWidgetItem *Item23 = new QTableWidgetItem(tr("%1ms").arg( qListProcessingTime[0] ));
	Item23->setBackground(QBrush(Qt::lightGray));
	tableWidget->setItem( uiImgCounter+2, 2, Item23 );
	QTableWidgetItem *Item24 = new QTableWidgetItem(tr("%1ms").arg( qListProcessingTime[uiImgCounter-2] ));
	Item24->setBackground(QBrush(Qt::lightGray));
	tableWidget->setItem( uiImgCounter+3, 2, Item24 );

	tableWidget->scrollToBottom();
}