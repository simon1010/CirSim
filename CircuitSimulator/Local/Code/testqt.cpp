#include "testqt.h"
#include <qpen.h>
#include <qrect.h>
#include <QGraphicsRectItem>
#include <qcursor.h>
#include <QMouseEvent>
#include <qdebug.h>

#include <set>

// Copyable final Includes
#include <Grid.h>
#include <GridUtils.h>
#include <STGRectangle.h>
#include <Comp_VoltageSource.h>
#include <ProgrammableVoltageSource.h>
#include <Resistor.h>
#include <Ground.h>
#include <Wire.h>
#include <Circuit.h>
#include <Speaker.h>
#include <Capacitor.h>
#include <qcustomplot.h>
#include <Microphone.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>

const unsigned int TestQt::sc_nGridSize = 10;

TestQt::TestQt(QWidget *parent)
  : QMainWindow(parent)
{ 
  ui.setupUi(this);
  //setCentralWidget(ui.graphicsView);
  
  mf_SetupCursor();
  SelectedElemenet = SimulationUtils::ElectronicElements::Element_Total;

  CGridUtils::sc_xTheGrid = new CGrid(parent, sc_nGridSize);
  ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  
  mf_SetupPlotter();
  mf_SetupFactory();
  mf_SetupLogger();
  mf_TestElements();

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
  //mv_SimulationTimer.setInterval();
  mv_SimulationTimer.start(0);
  connect(&mv_SimulationTimer, SIGNAL(timeout()), this, SLOT(update()));

  // Handle saving
  new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SLOT(ms_xSaveProject()));

  // Handle loading
  new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this, SLOT(ms_xOpenProject()));

}

TestQt::~TestQt()
{
  RunProcessing = false;
  if(ProcessingCoreThread.joinable())
    ProcessingCoreThread.join();
  Sleep(1000);
}

void TestQt::mf_SetupFactory()
{
  TypeMap["CResistor"]                  = &createInstance<CResistor>;
  TypeMap["CVoltageSource"]             = &createInstance<CVoltageSource>;
  TypeMap["CCapacitor"]                 = &createInstance<CCapacitor>;
  TypeMap["CMicrophone"]                = &createInstance<CMicrophone>;
  TypeMap["CProgrammableVoltageSource"] = &createInstance<CProgrammableVoltageSource>;
  TypeMap["CSpeaker"]                   = &createInstance<CSpeaker>;
}

void TestQt::mf_SetupPlotter()
{
  ui.Plotter->addGraph();
  ui.Plotter->graph(0)->setPen(QPen(Qt::red, 1)); // line color red for graph
  //ui.Plotter->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
  ui.Plotter->graph(0)->setAdaptiveSampling(true);
  //ui.Plotter->graph(0)->setAntialiasedFill(false);
 
 // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
  ui.Plotter->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  ui.Plotter->xAxis->setDateTimeFormat("hh:mm:ss");
  ui.Plotter->xAxis->setAutoTickStep(false);
  ui.Plotter->xAxis->setTickStep(2);
  ui.Plotter->axisRect()->setupFullAxesBox();
  // make left and bottom axes always transfer their ranges to right and top axes:
  connect(ui.Plotter->xAxis, SIGNAL(rangeChanged(QCPRange)), ui.Plotter->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui.Plotter->yAxis, SIGNAL(rangeChanged(QCPRange)), ui.Plotter->yAxis2, SLOT(setRange(QCPRange)));
  // Note: we could have also just called ui.Plotter->rescaleAxes(); instead
  // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
  ui.Plotter->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}



// continuous loop
void TestQt::update()
{
  int Elapsed = SimulationUtils::SimulationTime.elapsed();
  QString Time;
  double key = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0; // seconds
  Time.sprintf("Absolute time %f", key);

 // /*test plot*/

  // calculate two new data points
  static double lastPointKey = 0;
  //int lv_nSamples = 1;
 // if (key - lastPointKey > 0.0001) // at most add point every .1 ms
  //{ 
    SAMPLE* value0 = new SAMPLE;
    int lv_nSamples = 1;
    value0[0] = -2.;
    if (nullptr != SimulationUtils::TheScopeValue)
    {
      value0 = SimulationUtils::TheScopeValue(lv_nSamples);
    }

    if (lv_nSamples > 1)
    {
      QVector<qreal> keys(lv_nSamples);
      QVector<qreal> values(lv_nSamples);
      for (int i = 0; i < lv_nSamples; ++i)
      {
        keys[i] = (key);
        key += (0.0001 / lv_nSamples);
        values[i] = (value0[i]);
      }
      ui.Plotter->graph(0)->addData(keys, values);
    }
    else
    {
      ui.Plotter->graph(0)->addData(key, value0[0]);
    }
       
    // remove data of lines that's outside visible range:
    ui.Plotter->graph(0)->removeDataBefore(key - 8);
   
    // rescale value (vertical) axis to fit the current data:
    ui.Plotter->graph(0)->rescaleValueAxis();
    lastPointKey = key;
 // }
  // make key axis range scroll with the data (at a constant range size of 8):
  //ui.Plotter->xAxis->setRange(key + 0.25, 8, Qt::AlignRight);
  ui.Plotter->xAxis->setRange(key + 0.025,8, Qt::AlignRight);
  ui.Plotter->replot();

  // calculate frames per second:
  static double lastFpsKey;
  static int frameCount;
  ++frameCount;
  if (key - lastFpsKey > 2) // average fps over 2 seconds
  {
    ui.statusbar->showMessage(
      QString("%1 FPS, Total Data points: %2")
      .arg(frameCount / (key - lastFpsKey), 0, 'f', 0)
      .arg(ui.Plotter->graph(0)->data()->count())
      , 0);
    lastFpsKey = key;
    frameCount = 0;
  }

 // /*end plot test*/

  mv_LiveViewTime.setPlainText(Time);
  mv_LiveViewLogger.setPlainText(CGridUtils::sc_xTheGrid->mf_szGetTheGridText());

  qApp->processEvents(QEventLoop::AllEvents);
}

void TestQt::resizeEvent(QResizeEvent * event)
{
  CGridUtils::sc_xTheGrid->setSceneRect(QRect(QPoint(20, 20), QSize(event->size())));
}

void TestQt::mf_SetupLogger()
{
  mv_LiveViewLogger.setPlainText(CGridUtils::sc_xTheGrid->mf_szGetTheGridText());// = { QGraphicsTextItem(CGridUtils::sc_xTheGrid->mf_szGetTheGridText()) };
 // mv_LiveViewTime
  
  QPoint graphicsViewPosition = mapToGlobal(ui.graphicsView->pos());
  QRect graphicsViewRect = ui.graphicsView->rect();
  double graphicsView_X_Boundary = graphicsViewPosition.x() + graphicsViewRect.width();
  double graphicsView_Y_Boundary = graphicsViewPosition.y() + graphicsViewRect.height();

  mv_LiveViewTime.setPos(QPointF(graphicsView_X_Boundary - 50, graphicsView_X_Boundary - 50));
  mv_LiveViewLogger.setPos(QPointF(graphicsView_X_Boundary - 30, graphicsView_X_Boundary - 30));

  CGridUtils::sc_xTheGrid->addItem(&mv_LiveViewTime);
  CGridUtils::sc_xTheGrid->addItem(&mv_LiveViewLogger);
}

void TestQt::mf_SetupCursor()
{
  // Create custom thin, new and shiny cursor :>
  // http://doc.qt.io/qt-4.8/qcursor.html
  
  setMouseTracking(true);
  _ASSERT(hasMouseTracking());

 // grabMouse(); // CAREFULLLLLLL
  
  QCursor lv_xGridCursor(Qt::ArrowCursor);
  setCursor(lv_xGridCursor);
}

void TestQt::sc_xWriteStatus(const char *ac_pMessage)
{
  ui.statusbar->showMessage(ac_pMessage);
}

void TestQt::ShowContextMenu(const QPoint& pos) // this is a slot
{
  // http://www.setnode.com/blog/right-click-context-menus-with-qt/
  QPoint globalPos = mapToGlobal(pos);
  setCursor(Qt::ArrowCursor);
  QMenu myMenu;
   
  myMenu.addAction("Selection", this, SLOT(ms_xSelectRectangle()));
  myMenu.addAction("DC Source", this, SLOT(ms_xSelectDCSource()));
  myMenu.addAction("Programmable VS", this, SLOT(ms_xSelectProgrammableVoltageSource()));
  myMenu.addAction("Wire", this, SLOT(ms_xSelectWire()));
  myMenu.addAction("Resistor", this, SLOT(ms_xSelectResistor()));
  myMenu.addAction("Ground", this, SLOT(ms_xSelectGround()));
  myMenu.addAction("Capacitor", this, SLOT(ms_xSelectCapacitor()));
  myMenu.addAction("Microphone", this, SLOT(ms_xSelectMicrophone()));
  myMenu.addAction("Speaker", this, SLOT(ms_xSelectSpeaker()));
  
  QAction* selectedItem = myMenu.exec(globalPos); 
}

void TestQt::mf_TestElements()
{ 
  ui.graphicsView->setScene(CGridUtils::sc_xTheGrid);

  IComponent *R1 = new CResistor(QPoint(390,280), CGridUtils::sc_xTheGrid, ui.graphicsView);
  CGridUtils::sc_xTheGrid->addItem(R1);

  IComponent *R2 = new CMicrophone(QPoint(270, 280), CGridUtils::sc_xTheGrid, ui.graphicsView);
  CGridUtils::sc_xTheGrid->addItem(R2);

  CWire *WR1N = new CWire(QPoint(300, 300), CGridUtils::sc_xTheGrid, ui.graphicsView);
  WR1N->setLine(QLineF(300, 300, 360, 300));
  CGridUtils::sc_xTheGrid->addItem(WR1N);

  CWire *WNR2 = new CWire(QPoint(360, 300), CGridUtils::sc_xTheGrid, ui.graphicsView);
  WNR2->setLine(QLineF(360, 300, 400, 300));
  CGridUtils::sc_xTheGrid->addItem(WNR2);

  IComponent *E2 = new CVoltageSource(QPoint(340, 290), CGridUtils::sc_xTheGrid, ui.graphicsView);
  CGridUtils::sc_xTheGrid->addItem(E2);

  static_cast<CVoltageSource*>(E2)->mf_SetVoltage(4.);

  IComponent *E1 = new CVoltageSource(QPoint(260, 290), CGridUtils::sc_xTheGrid, ui.graphicsView);
  CGridUtils::sc_xTheGrid->addItem(E1);

  CWire *WR2N = new CWire(QPoint(420, 290), CGridUtils::sc_xTheGrid, ui.graphicsView);
  WR2N->setLine(QLineF(420, 300, 420, 350));
  CGridUtils::sc_xTheGrid->addItem(WR2N);

  CWire *WNDC = new CWire(QPoint(420, 350), CGridUtils::sc_xTheGrid, ui.graphicsView);
  WNDC->setLine(QLineF(420, 350, 360, 350));
  CGridUtils::sc_xTheGrid->addItem(WNDC);

  CWire *WNDCE1 = new CWire(QPoint(360, 350), CGridUtils::sc_xTheGrid, ui.graphicsView);
  WNDCE1->setLine(QLineF(360, 350, 280, 350));
  CGridUtils::sc_xTheGrid->addItem(WNDCE1);
  
  CWire *WDCE1 = new CWire(QPoint(280, 350), CGridUtils::sc_xTheGrid, ui.graphicsView);
  WDCE1->setLine(QLineF(280, 350, 280, 320));
  CGridUtils::sc_xTheGrid->addItem(WDCE1);

  CWire *WNDCE2 = new CWire(QPoint(360, 350), CGridUtils::sc_xTheGrid, ui.graphicsView);
  WNDCE2->setLine(QLineF(360, 350, 360, 320));
  CGridUtils::sc_xTheGrid->addItem(WNDCE2);

 // ui.graphicsView->centerOn(0,0); // move scrollbar to show 0 , 0
}

void TestQt::pushBtn()
{}

void TestQt::ms_xSelectProgrammableVoltageSource()
{
  SelectedElemenet = SimulationUtils::ElectronicElements::Element_ProgrammableVoltageSource;
  ui.statusbar->showMessage("Element_Programmable_Voltage_Source");
}

void TestQt::ms_xSelectRectangle()
{
  mf_EnableSelection();
  SelectedElemenet = SimulationUtils::ElectronicElements::Element_Total;
  ui.statusbar->showMessage("Element_Total:nothing");
}

void TestQt::ms_xSelectDCSource()
{
  SelectedElemenet = SimulationUtils::ElectronicElements::Element_DC_Source;
  ui.statusbar->showMessage("Element_DC_Source");
}

void TestQt::ms_xSelectWire()
{ 
  mf_DisableSelection();
  setCursor(Qt::CrossCursor);
  SelectedElemenet = SimulationUtils::ElectronicElements::Element_Wire;
  ui.statusbar->showMessage("Element_Wire");
}

void TestQt::ms_xSelectResistor()
{
  SelectedElemenet = SimulationUtils::ElectronicElements::Element_Resistor;
  ui.statusbar->showMessage("Element_Resistor");
}

void TestQt::ms_xSelectGround()
{
  SelectedElemenet = SimulationUtils::ElectronicElements::Element_Ground;
  ui.statusbar->showMessage("Element_Ground");
}

void TestQt::ms_xSelectCapacitor()
{
  SelectedElemenet = SimulationUtils::ElectronicElements::Element_Capacitor;
  ui.statusbar->showMessage("Element_Capacitor");
}

void TestQt::ms_xSelectSpeaker()
{
  SelectedElemenet = SimulationUtils::ElectronicElements::Element_Speaker;
  ui.statusbar->showMessage("Element_Capacitor");
}

void TestQt::ms_xSelectMicrophone()
{
  SelectedElemenet = SimulationUtils::ElectronicElements::Element_Microphone;
  ui.statusbar->showMessage("Element_Microphone");
}

void TestQt::mf_DisableSelection()
{
  QList<QGraphicsItem *> lv_AllElements = CGridUtils::sc_xTheGrid->items(CGridUtils::sc_xTheGrid->itemsBoundingRect());
  for (QGraphicsItem * it : lv_AllElements)
  {
    it->setFlag(QGraphicsItem::ItemIsMovable, false);
    it->setFlag(QGraphicsItem::ItemIsSelectable, false);
  }
}

void TestQt::mf_EnableSelection()
{
  QList<QGraphicsItem *> lv_AllElements = CGridUtils::sc_xTheGrid->items(CGridUtils::sc_xTheGrid->itemsBoundingRect());
  for (QGraphicsItem * it : lv_AllElements)
  {
    it->setFlag(QGraphicsItem::ItemIsMovable, true);
    it->setFlag(QGraphicsItem::ItemIsSelectable, true);
  }
}


void TestQt::mousePressEvent(QMouseEvent * event)
{
  // store first position
  /*event->localPos();
  QPoint GlobalPosition(event->x(), event->y());
  char MousePositionLogging[100];

  QPoint pos(GlobalPosition);
  sprintf(MousePositionLogging, "Source at < %d | %d >", pos.x(), pos.y());
  ui.statusbar->showMessage(MousePositionLogging);

  CVoltageSource *lv_DCVoltageSource = new CVoltageSource(pos, CGridUtils::sc_xTheGrid, nullptr);
  CResistor *lv_xResistor = new CResistor(QRectF(QPoint(pos),QSize(20,20)), nullptr);
  CGridUtils::sc_xTheGrid->addItem(lv_DCVoltageSource);*/

  if (event->button() == Qt::LeftButton)
  {
    
      QPoint lv_xElementPosition(event->x(), event->y());
      IComponent *Element;

      char lv_szLogging[100];
      sprintf_s(lv_szLogging, "ELEMENT AT < %d | %d >", event->x(), event->y());

      sc_xWriteStatus(lv_szLogging);

      switch (SelectedElemenet)
      {
      case SimulationUtils::ElectronicElements::Element_Resistor:
        Element = new CResistor(lv_xElementPosition, CGridUtils::sc_xTheGrid, ui.graphicsView);
        break;
      case SimulationUtils::ElectronicElements::Element_DC_Source:
        Element = new CVoltageSource(lv_xElementPosition, CGridUtils::sc_xTheGrid, ui.graphicsView);
        break;
      case SimulationUtils::ElectronicElements::Element_Wire:
        grabMouse();
        mv_SpawnWire = new CWire(lv_xElementPosition, CGridUtils::sc_xTheGrid, ui.graphicsView);
        CGridUtils::sc_xTheGrid->addItem(mv_SpawnWire);
        mv_LineStartsFrom = new QPointF(lv_xElementPosition.x() + 15, lv_xElementPosition.y() + 15);
        QMainWindow::mousePressEvent(event);
        return;
        break;
      case SimulationUtils::ElectronicElements::Element_Total:
        grabMouse();
        rubberBandStartsFrom = QPoint(event->pos());
        rubberBand = new QRubberBand(QRubberBand::Rectangle);//new rectangle band
        rubberBand->setGeometry(QRect(mapToGlobal(rubberBandStartsFrom), QSize(1, 1)));
        rubberBandItemSelectionRectangle = QRect(rubberBandStartsFrom, QSize(1, 1));
        rubberBand->show();
        QMainWindow::mousePressEvent(event);
        return;
        break;
      case SimulationUtils::ElectronicElements::Element_Ground:
        Element = new CGround(lv_xElementPosition, CGridUtils::sc_xTheGrid, ui.graphicsView);
        break;
      case SimulationUtils::ElectronicElements::Element_ProgrammableVoltageSource:
        Element = new CProgrammableVoltageSource(lv_xElementPosition, CGridUtils::sc_xTheGrid, ui.graphicsView);
        break;
      case SimulationUtils::ElectronicElements::Element_Capacitor:
        Element = new CCapacitor(lv_xElementPosition, CGridUtils::sc_xTheGrid, ui.graphicsView);
        break;
      case SimulationUtils::ElectronicElements::Element_Microphone:
        Element = new CMicrophone(lv_xElementPosition, CGridUtils::sc_xTheGrid, ui.graphicsView);
        break;
      case SimulationUtils::ElectronicElements::Element_Speaker:
        Element = new CSpeaker(lv_xElementPosition, CGridUtils::sc_xTheGrid, ui.graphicsView);
        break;
      default:
        Element = new CVoltageSource(lv_xElementPosition, CGridUtils::sc_xTheGrid, ui.graphicsView);
        break;
      }
      CGridUtils::sc_xTheGrid->addItem(Element);
    }
  
  QMainWindow::mousePressEvent(event);
 
}

void TestQt::mouseMoveEvent(QMouseEvent * event)
{  
  /* handle rubberband event */
  if (SelectedElemenet == SimulationUtils::Element_Total && (nullptr != rubberBand))
  {
    QPoint eventGlobalPosition = mapToGlobal(event->pos());
    QPoint graphicsViewPosition = mapToGlobal(ui.graphicsView->pos());
    QRect graphicsViewRect = ui.graphicsView->rect();
    double graphicsView_X_Boundary = graphicsViewPosition.x() + graphicsViewRect.width();
    double graphicsView_Y_Boundary = graphicsViewPosition.y() + graphicsViewRect.height();
    
    double MouseBoundary_x, MouseBoundary_y;

    if (eventGlobalPosition.x() < graphicsView_X_Boundary)
      MouseBoundary_x = eventGlobalPosition.x();
    else if (eventGlobalPosition.x() < graphicsViewPosition.x())
    {
      MouseBoundary_x = graphicsViewPosition.x();
    }
    else
    {
      MouseBoundary_x = graphicsView_X_Boundary;
    }

    if (eventGlobalPosition.y() < graphicsView_Y_Boundary)
      MouseBoundary_y = eventGlobalPosition.y();
    else
    {      
      MouseBoundary_y = graphicsView_Y_Boundary;
    }
    
    if (eventGlobalPosition.x() < graphicsViewPosition.x())
    {
      MouseBoundary_x = graphicsViewPosition.x();
    }
    if (eventGlobalPosition.y() < graphicsViewPosition.y())
    {
      MouseBoundary_y = graphicsViewPosition.y();
    }
    
    //QCursor::setPos(QPoint(MouseBoundary_x, MouseBoundary_y));
    rubberBand->setGeometry(QRect(mapToGlobal(rubberBandStartsFrom), mapToGlobal(event->pos())).normalized());
    rubberBandItemSelectionRectangle = QRect(rubberBandStartsFrom, event->pos());
  }
  /* End rubberband event handling */

  if (SelectedElemenet == SimulationUtils::Element_Wire)
  {
    //CGridUtils::sf_xCursorToGrid();
    
    /* Line operations */
    mv_LineEndAt = new QPointF(event->pos().x() + 10, event->pos().y() + 10);
    mv_SpawnWire->setLine(QLineF(*mv_LineStartsFrom, *mv_LineEndAt));
  }
  char lv_szLogging[100];
  sprintf_s(lv_szLogging, "Cursor < %d | %d >", /*QCursor::pos().x()*/event->pos().x(), /*QCursor::pos().y()*/event->pos().y());

  sc_xWriteStatus(lv_szLogging);

  QMainWindow::mouseMoveEvent(event);
}

void TestQt::mouseReleaseEvent(QMouseEvent * event)
{
  // determine selection, for example using QRect::intersects()
  // and QRect::contains().
  
  if (SelectedElemenet == SimulationUtils::Element_Total)
  {
    // TODO : Must Revisit when time
    const QRect lc_RubberBandRectangle = rubberBandItemSelectionRectangle;//rubberBand->rect();
    const QPoint lc_RubberBandPosition = mapTo(this, QPoint(rubberBandItemSelectionRectangle.x(), rubberBandItemSelectionRectangle.y())); //mapTo(this, rubberBand->pos());
   
    mv_SelectedItems = CGridUtils::sc_xTheGrid->items(
      qreal(lc_RubberBandPosition.x()),
      qreal(lc_RubberBandPosition.y()),
      qreal(lc_RubberBandRectangle.width()),
      qreal(lc_RubberBandRectangle.height()),
      Qt::ItemSelectionMode::ContainsItemBoundingRect,
      Qt::SortOrder::AscendingOrder);

    for (QGraphicsItem *it : mv_SelectedItems)
    {
      it->setSelected(true);
    }
   
    char log[100];
    sprintf(log, "Items: %d", mv_SelectedItems.length());
    sc_xWriteStatus(log);
    
    releaseMouse();
    rubberBand->hide();// hide on mouse Release
    rubberBand->clearMask();
    
    // Grab keyboard only if the rubber band has a size.
    // Otherwise the circuit components cannot handle key-press events themselves
    auto const lc_RubberBandSize = rubberBand->size();
    if( lc_RubberBandSize.width() > 5 || lc_RubberBandSize.height() > 5 )
      grabKeyboard();
  }

  if (SelectedElemenet == SimulationUtils::Element_Wire)
  {    
    releaseMouse();
   // CGridUtils::sc_xTheGrid->addItem(mv_SpawnWire);
  }

  QMainWindow::mouseReleaseEvent(event);
}

void TestQt::ms_xSaveProject()
{
  auto fileName = QFileDialog::getSaveFileName(this, tr("Save Project File"), "c:\\", tr("JSON File (*.json)"));

  SimulationUtils::sf_PrintElementsToFile(CGridUtils::sc_xTheGrid->items(CGridUtils::sc_xTheGrid->itemsBoundingRect()), fileName.toStdString().c_str());//"c:\\CIRSIM_LOGGING\\Project.json");
}

void TestQt::ms_xOpenProject()
{
  RunProcessing = false;
  auto fileName = QFileDialog::getOpenFileName(this, tr("Open Project File"), "c:\\", tr("JSON File (*.json *.JSON)"));

  // todo clear grid! & file name correctness!
  QFile loadFile(fileName);

  if (!loadFile.open(QIODevice::ReadOnly)) {
    qWarning("Couldn't open save file.");
    return;
  }
  
  CGridUtils::sc_xTheGrid->removeItem(&mv_LiveViewTime);
  CGridUtils::sc_xTheGrid->removeItem(&mv_LiveViewLogger);
  CGridUtils::sc_xTheGrid->clear();
  mf_SetupLogger();

  QByteArray lv_SavedData = loadFile.readAll();
  QJsonDocument lv_LoadedDoc(QJsonDocument::fromJson(lv_SavedData));
  QJsonObject lv_JSonObject = lv_LoadedDoc.object();
  QJsonArray lv_ComponentArray = lv_JSonObject["components"].toArray();
  for (int CompIdx = 0; CompIdx < lv_ComponentArray.size(); ++CompIdx)
  {  
    QJsonObject ComponentJSonObject = lv_ComponentArray[CompIdx].toObject();
    QString lv_ComponentType = ComponentJSonObject["name"].toString();
    if (lv_ComponentType == QString("CWire"))
    {
      auto x1 = ComponentJSonObject["positionX1"].toInt();
      auto y1 = ComponentJSonObject["positionY1"].toInt();

      auto x2 = ComponentJSonObject["positionX2"].toInt();
      auto y2 = ComponentJSonObject["positionY2"].toInt();

      CWire * lv_newWire = new CWire(QPoint(x1, y2), CGridUtils::sc_xTheGrid, ui.graphicsView);
      lv_newWire->setLine(QLineF(x1, y1, x2, y2));
      CGridUtils::sc_xTheGrid->addItem(lv_newWire);
    }
    else
    {
      auto x = ComponentJSonObject["positionX"].toInt()- 12;
      auto y = ComponentJSonObject["positionY"].toInt()- 12;

      IComponent* lv_Component = TypeMap[lv_ComponentType.toStdString()](QPointF(x, y), CGridUtils::sc_xTheGrid, ui.graphicsView);
      CGridUtils::sc_xTheGrid->addItem(lv_Component);
      lv_Component->mf_Load(ComponentJSonObject);
    }
  }
}

void TestQt::DoCircuitUpdate()
{
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
  while (RunProcessing)
  {
    if (mv_CircuitComposer != nullptr)
      mv_CircuitComposer->step();
    else
      Sleep(10);
  }
}

void TestQt::keyPressEvent(QKeyEvent *event)
{
  switch (event->key())
  {
  case Qt::Key_Delete:
  case Qt::Key_Backspace:
    for (auto it : mv_SelectedItems)
    {
      CGridUtils::sc_xTheGrid->removeItem(it);
    }
    mv_SelectedItems.clear();
    releaseKeyboard();
    break;
  case Qt::Key_S:
    //SimulationUtils::SimulationTime = QTime::currentTime();
    SimulationUtils::SimulationTime.start();

    auto const lc_GUICircuitElements = CGridUtils::sc_xTheGrid->items(CGridUtils::sc_xTheGrid->itemsBoundingRect());
    
    Solver::Circuit TheCircuit(lc_GUICircuitElements);
    const unsigned int lc_nNrofConnectedObjects = TheCircuit.mf_nConnectedElements();

    mv_CircuitComposer = make_shared<Solver::CCircuitComposer>(lc_GUICircuitElements, TheCircuit.mf_AllEdgesInCircuit(), TheCircuit.mf_AllNodesInCircuit());

    if (RunProcessing)
    {
      RunProcessing = false;
      if (ProcessingCoreThread.joinable())
        ProcessingCoreThread.join();
    }

    RunProcessing = true;
    ProcessingCoreThread = std::thread(&TestQt::DoCircuitUpdate, this);
    break;
  }
}

// http://www.qtcentre.org/archive/index.php/t-6640.html
// http ://www.thinkgeek.com/geektoys/science/

//http ://www.thedigitalmachine.net/eispice.html < ----- SOLVER
// http://crd-legacy.lbl.gov/~xiaoye/SuperLU/ < -- acelasi, dar in genunchi

//http://zetcode.com/gui/pyqt4/ < ----- irelevant poate

//https://docs.python.org/2/extending/embedding.html < ------- important

//http://www.dreamincode.net/forums/topic/127903-spice-solver-using-c/

//http://stackoverflow.com/questions/607297/analog-circuit-simulation-library
/*
Ideea:

take eispice, use the sorver to solve the matrix supplied by our GUI, forward output to a python widget with pyqt4

*/