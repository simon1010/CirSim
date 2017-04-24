#include <IComponent.h>
#include <qapplication.h>
#include <Grid.h>
#include <GridUtils.h>
#include <qgraphicswidget.h>
#include <QGraphicsSceneMouseEvent>

#include <qslider.h>

unsigned int IComponent::sv_nCompNumber = 0;

IComponent::IComponent( QGraphicsScene* ac_pScene, QGraphicsView *ac_pPrent) : QGraphicsItem(nullptr),
mv_pParent(ac_pPrent)
{
  ++sv_nCompNumber;

  // Must Revisit, check flags
  setFlags(QGraphicsItem::ItemIsSelectable |
    QGraphicsItem::ItemIsMovable |
    QGraphicsItem::ItemSendsGeometryChanges);

  setAcceptHoverEvents(true);
  
  mv_bHasKeyboard = false;
}

IComponent::IComponent(QPointF ac_Position, QGraphicsScene* ac_pScene, QGraphicsView *ac_pPrent) : QGraphicsItem(nullptr),
mv_pParent(ac_pPrent)
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

  mv_bHasKeyboard = false;
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
    if (mv_bHasKeyboard)
    {
      mv_bHasKeyboard = false;
      ungrabKeyboard();
    }
    else
    {
      mv_bHasKeyboard = true;
      grabKeyboard();
    }      
  }

  if ((change == ItemPositionChange || change == ItemPositionHasChanged) && scene())
  {
    QPointF SceneCorner = mv_pParent->mapToGlobal(mv_pParent->pos());
 
    QPointF MousePosition = QCursor::pos();
    QPointF newPos(MousePosition.x() - SceneCorner.x() + 15, MousePosition.y() - SceneCorner.y() + 15); // value.toPointF(); 
    
    if (QApplication::mouseButtons() == Qt::LeftButton && (nullptr != CGridUtils::sc_xTheGrid))
    {
      const unsigned lc_nGridSize = CGridUtils::sc_xTheGrid->mf_nGetGridSize();
      QVariant ReferenceVariant = QGraphicsItem::itemChange(change, value);
      return CGridUtils::sf_xAdjustPositionToGrid(newPos);
    }
    else 
    {
      return newPos;
    }
  }  
  else
    return QGraphicsItem::itemChange(change, value);
}


/*   if (pChange == ItemPositionChange && scene())
    {
        // value is the new position.
        QPointF newPos = pValue.toPointF();
        QRectF rect = scene()->sceneRect();
        if (!rect.contains(newPos))
        {
            double rectBottom = rect.bottom() + MAP_SCENE_DIFF;
            // Keep the item inside the scene rect.
            double posX = qMin(rect.right(), qMax(newPos.x(), rect.left()));
            double posY = qMin(rectBottom, qMax(newPos.y(), rect.top()));
            newPos = QPointF(posX, posY);

            return newPos;
        }
    }
    else if(pChange == ItemPositionHasChanged)
    {
        QPointF endPoint(pValue.toPointF().x(), pValue.toPointF().y());
        QPointF startPoint = mpParent->pos();
        mpCombineLine->setLine(QLineF(startPoint, endPoint));

        return QPointF(pValue.toPointF().x(), pValue.toPointF().y());
    }

    return QGraphicsItem::itemChange(pChange, pValue);*/

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

void IComponent::keyPressEvent(QKeyEvent *event)
{
  if (this->isSelected())
  {
    switch (event->key())
    {
    case Qt::Key_Delete :
      mv_bHasKeyboard = false;
      ungrabKeyboard();
      CGridUtils::sc_xTheGrid->removeItem(this);
      mf_PreDestroy();
      break;
    }
  }
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
  using std::placeholders::_1;
  SimulationUtils::TheScopeValue = std::bind(&IComponent::mf_dfGetVoltage, this, _1);
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
