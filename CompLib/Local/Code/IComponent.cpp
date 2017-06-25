#include <IComponent.h>
#include <qapplication.h>
#include <Grid.h>
#include <GridUtils.h>
#include <qgraphicswidget.h>
#include <QGraphicsSceneMouseEvent>

#include <qslider.h>

unsigned int IComponent::sv_nCompNumber = 0;

IComponent::IComponent( QGraphicsScene* ac_pScene, QGraphicsView *ac_pPrent, const int ac_nPorts) 
: QGraphicsItem(nullptr)
, IDispatchComponent(ac_nPorts)
, mv_pParent(ac_pPrent)
{
  ++sv_nCompNumber;

  // Must Revisit, check flags
  setFlags(QGraphicsItem::ItemIsSelectable |
    QGraphicsItem::ItemIsMovable |
    QGraphicsItem::ItemSendsGeometryChanges);

  setAcceptHoverEvents(true);
}

IComponent::IComponent(QPointF ac_Position, QGraphicsScene* ac_pScene, QGraphicsView *ac_pPrent, const int ac_nPorts)
: QGraphicsItem(nullptr)
, IDispatchComponent(ac_nPorts)
, mv_pParent(ac_pPrent)
{
  ++sv_nCompNumber;
  // Must Revisit, check flags
  setFlags(QGraphicsItem::ItemIsSelectable |
    QGraphicsItem::ItemIsMovable |
    QGraphicsItem::ItemSendsGeometryChanges | ItemIgnoresTransformations );
  
  //setZValue(TRACK_Z_VALUE);
  //setFlags(ItemIsMovable | ItemSendsGeometryChanges | ItemIgnoresTransformations);
  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  setPos(CGridUtils::sf_xAdjustPositionToGrid(ac_Position));
}

IComponent::~IComponent()
{
  mf_PreDestroy();
}

void IComponent::mf_PreDestroy()
{

}

void IComponent::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
  QPoint globalPos = event->screenPos();
  QMenu myMenu;

  myMenu.addAction("View in Scope", this, SLOT(ms_xViewInScope()));
  myMenu.addAction("Properties", this, SLOT(ms_xPropertiesDialog()));
  myMenu.addAction("Remove", this, SLOT(ms_xRemove()));

  QAction* selectedItem = myMenu.exec(globalPos);
}

void IComponent::paint(QPainter *lc_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent)
{
 
  if (this->isSelected())
  {
    lc_pPainter->setPen(QPen(QColor(Qt::blue)));
  }

  mf_DerivedPaint(lc_pPainter, ac_pOption, ac_pParent);
}

QVariant IComponent::itemChange(GraphicsItemChange change, const QVariant &value)
{
  if (change == ItemSelectedChange)
  {
    bool lv_isOk = false;
    int selectionValue = value.toInt(&lv_isOk);
    
    if (lv_isOk && selectionValue == 1)
      grabKeyboard();
    else if (lv_isOk && selectionValue == 0)
      ungrabKeyboard();
  }


  if ((change == ItemPositionChange || change == ItemPositionHasChanged) && scene())
  {
    QVariant lv_UpperChangeReturn = __super::itemChange(change, value);
    if(lv_UpperChangeReturn.canConvert<QPointF>())
    {
      QPointF lv_RecalculatedPoint = lv_UpperChangeReturn.value<QPointF>();
      return CGridUtils::sf_xAdjustPositionToGrid(lv_RecalculatedPoint);
    }
  }

  return __super::itemChange(change, value);
}

void IComponent::mousePressEvent(QGraphicsSceneMouseEvent *ac_pMouseEvent)
{
  //update();
  if (ac_pMouseEvent->type() == QGraphicsSceneMouseEvent::GraphicsSceneMouseDoubleClick)
  {
    mf_ShowSettingsDialog();    
  }
  switch (ac_pMouseEvent->button())
  {
  case Qt::MouseButton::LeftButton:
    CGridUtils::sc_xTheGrid->mf_SetGridText(mf_ToolTipGetType() + mf_ToolTipGetValue() + mf_ToolTipGetUnit());
    break;
  default:
    break;
  }  
  QGraphicsItem::mousePressEvent(ac_pMouseEvent);
}

void IComponent::mouseReleaseEvent(QGraphicsSceneMouseEvent *ac_pMouseEvent)
{
  //update();
  QGraphicsItem::mouseReleaseEvent(ac_pMouseEvent);
}

void IComponent::update(const QRectF &ac_Rect)
{
  QGraphicsItem::update(ac_Rect);
}

const QString IComponent::mf_szGetCompName() const
{
  return mv_sCompName;
}

const QString IComponent::mf_szGetCompID() const
{
  auto const IDString = SimulationUtils::string_format("%s%nN%n", mf_szGetCompName(), mf_nGetInstanceNumber(), sv_nCompNumber);
  return QString(IDString.data());
}

void IComponent::keyPressEvent(QKeyEvent *event)
{
  if (this->isSelected())
  {
    switch (event->key())
    {
    case Qt::Key_Backspace:
    case Qt::Key_Delete:
    {
      mv_bHasKeyboard = false;
      CGridUtils::sc_xTheGrid->removeItem(this);
      mf_PreDestroy();
    } break;
    case Qt::Key_R:
    {
      this->setRotation(this->rotation() + 90);
    } break;
    }
  }
}

Terminal* IComponent::mf_GetTerminal(const int ac_nIdx) const
{
  auto begin = mv_Terminals.begin();
  auto end = mv_Terminals.end();
  for (int i = 0; i < mv_Terminals.size() && begin != end; i++, begin++)
  {
    if (i == ac_nIdx)
      return *begin;
  }
  return nullptr;
}

void IComponent::mf_AddTerminal(Terminal *ac_Terminal)
{
  mv_Terminals.insert(ac_Terminal);
}

Terminal* IComponent::mf_OtherTerminal(Terminal * ac_NotThisTerminal)const
{
  Terminal *lv_OtherTerminal;
  QSet<Terminal *>::const_iterator lv_NotThisTerminalIterator = mv_Terminals.find(ac_NotThisTerminal);
  ++lv_NotThisTerminalIterator;
  if (lv_NotThisTerminalIterator == mv_Terminals.end())
  {
    return *(mv_Terminals.begin());
  }
  else
  {
    return *lv_NotThisTerminalIterator;
  }
}

void IComponent::hoverEnterEvent(QGraphicsSceneHoverEvent *ac_Event)
{
  setToolTip(mf_ToolTipGetType() + mf_ToolTipGetValue() + mf_ToolTipGetUnit());
}

IComponent::DialogSettingsMap IComponent::mf_GetDialogSettingsMap()
{
  _ASSERT(!"Not implemented for this component!");
  DialogSettingsMap lv_Variables;
  return lv_Variables;
}

void IComponent::mf_SetDialogSettingsMap(IComponent::DialogSettingsMap ac_SettingsMap)
{
 _ASSERT(!"Not implemented for this component!");
}

QString intToQString(int ac_nValue)
{
  QString QStringValue;
  QStringValue.sprintf("%d", ac_nValue);
  return QStringValue;
}

void IComponent::ms_xSliderValueChanged(int ac_nValue)
{  
  qobject_cast<ComponentDialog*>(mv_SettingsDialog)->lineEdit->setText(intToQString(ac_nValue));
}

void IComponent::mf_ShowSettingsDialog()
{
  mv_SettingsDialog = new ComponentDialog();
  DialogSettingsMap lv_DialogSettings = mf_GetDialogSettingsMap();
  if (lv_DialogSettings.size())
  {
    // future support multiple variable settings
    DialogSettingsMap::iterator begin = lv_DialogSettings.begin();

    // wet dream
    //mv_SettingsDialog->mf_SetupContents(mf_GetDialogSettingsMap());
    mv_SettingsDialog = qobject_cast<ComponentDialog*>(mv_SettingsDialog);
    QByteArray array = begin->first.toLocal8Bit();
    qobject_cast<ComponentDialog*>(mv_SettingsDialog)->SliderLabel->setTitle(QApplication::translate("Dialog", array.data(), 0));
    qobject_cast<ComponentDialog*>(mv_SettingsDialog)->horizontalSlider->setRange((begin->second / 2), (begin->second * 4));
    qobject_cast<ComponentDialog*>(mv_SettingsDialog)->horizontalSlider->setSliderPosition(begin->second);
    qobject_cast<ComponentDialog*>(mv_SettingsDialog)->lineEdit->setText(intToQString(begin->second));

    QObject::connect(qobject_cast<ComponentDialog*>(mv_SettingsDialog)->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(ms_xSliderValueChanged(int)));
    if (mv_SettingsDialog->exec())
    {
      // Ok pressed, apply settings to component
      begin->second = qobject_cast<ComponentDialog*>(mv_SettingsDialog)->lineEdit->text().toDouble();
      mf_SetDialogSettingsMap(lv_DialogSettings);
    }
    else
    {
      // cancel
    }
  }
}

void IComponent::ms_xViewInScope()
{
  SimulationUtils::TheScopeValue = std::bind(&IComponent::mf_dfGetVoltage, this, std::placeholders::_1, std::placeholders::_2);
}

void IComponent::ms_xPropertiesDialog()
{
  mf_ShowSettingsDialog();
}

void IComponent::ms_xRemove()
{
  CGridUtils::sc_xTheGrid->removeItem(this);
  mf_PreDestroy();
}

void IComponent::mf_Load(QJsonObject & json)
{
}