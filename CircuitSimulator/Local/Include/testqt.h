#ifndef TESTQT_H
#define TESTQT_H

#include <QtWidgets/QMainWindow>
#include "ui_testqt.h"
#include <Wire.h>
#include <Grid.h>
#include <IComponent.h>
#include <QRubberBand>
#include <SimulationUtils.h>
#include <QTimer>
#include <CircuitComposer.h>
// Component factory
template<typename T> IComponent * createInstance(QPointF& p, CGrid * g, QGraphicsView * mw) { return new T(p, g, mw); }
typedef std::map<std::string, IComponent*(*)(QPointF&, CGrid *, QGraphicsView *)> map_type;

class TestQt : public QMainWindow
{
  Q_OBJECT

public:
  TestQt(QWidget *parent = 0);
  ~TestQt();

  void sc_xWriteStatus(const char *ac_pMessage);
  
  
  /* Live operations! Carefull! */
public slots:
  void update();
  
private:
  // Component factory
  void TestQt::mf_SetupFactory();
  map_type TypeMap;

  // DSP
  shared_ptr<Solver::CCircuitComposer> mv_CircuitComposer;
  // DSP 
 
  QTimer mv_SimulationTimer;
  Ui_MainWindow ui;

  /* const static members to be moved to a configuration dialog and configuration file */
  const static unsigned sc_nGridSize;
  SimulationUtils::ElectronicElements SelectedElemenet;
  
  /* Rubberband selection aid */
  QRubberBand *rubberBand;
  QPoint rubberBandStartsFrom;
  QRect  rubberBandItemSelectionRectangle;


  /* Wire aid */
  QPointF *mv_LineStartsFrom;
  QPointF *mv_LineEndAt;
  CWire *mv_SpawnWire;

  /* Element Selection Handling*/
  QList<QGraphicsItem*> mv_SelectedItems;

  /* On-screen text */
  QGraphicsTextItem mv_LiveViewLogger; 
  QGraphicsTextItem mv_LiveViewTime;
  void mf_SetupLogger();

  /* Processing */
  void mf_SetupPlotter();

  void mf_TestElements();
  void mf_SetupCursor();
  void mf_EnableSelection();
  void mf_DisableSelection();
 
  void mousePressEvent(QMouseEvent * event);
  void mouseMoveEvent(QMouseEvent * event);
  void mouseReleaseEvent(QMouseEvent * event);
  void resizeEvent(QResizeEvent * event);
  void keyPressEvent(QKeyEvent * event);

  private slots:
  void pushBtn();
  void ShowContextMenu(const QPoint& pos);
  
  void ms_xSaveProject();
  void ms_xOpenProject();

  void ms_xSelectRectangle();
  void ms_xSelectDCSource(); 
  void ms_xSelectWire();
  void ms_xSelectResistor();
  void ms_xSelectGround();
  void ms_xSelectProgrammableVoltageSource();
  void ms_xSelectCapacitor();
  void ms_xSelectMicrophone();
  void ms_xSelectSpeaker();
};



#endif // TESTQT_H
