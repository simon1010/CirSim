//#include <Wire.h>
//#include <qgraphicssceneevent.h>
//#include <QPainter>
//#include <GridUtils.h>
//#include <qapplication.h>
//
//
//CWire::CWire(QPointF ac_Position, QGraphicsScene *ac_pScene, QGraphicsView *ac_pParent) :mv_pParent(ac_pParent)// :IComponent(ac_Position, ac_pScene, ac_pParent)
//{
//  setFlags(QGraphicsItem::ItemIsSelectable |
//    QGraphicsItem::ItemIsMovable |
//    QGraphicsItem::ItemSendsGeometryChanges );
//
//  setCacheMode(QGraphicsItem::DeviceCoordinateCache);
//
//  mv_bHasKeyboard = false;
//
//  mv_Terminal_1 = new Terminal(ac_pScene, this);
//  mv_Terminal_2 = new Terminal(ac_pScene, this);
//}
//
//CWire::~CWire()
//{
//}
//
//QVariant CWire::itemChange(GraphicsItemChange change, const QVariant &value)
//{
//  if (change == ItemSelectedChange)
//  {
//    if (mv_bHasKeyboard)
//    {
//      mv_bHasKeyboard = false;
//      ungrabKeyboard();
//    }
//    else
//    {
//      mv_bHasKeyboard = true;
//      grabKeyboard();
//    }
//  }
//  if ((change == ItemPositionChange || change == ItemPositionHasChanged) && scene())
//  {  
//    if (QApplication::mouseButtons() == Qt::LeftButton)
//    {
//      return CGridUtils::sf_xAdjustPositionToGrid(value.toPointF());
//    }
//    else
//    {
//      return value.toPointF();
//    }
//  }
//  else
//    return QGraphicsItem::itemChange(change, value);
//}
//
//QRectF CWire::boundingRect() const
//{
//  return QGraphicsLineItem::boundingRect();
//}
//
//void CWire::paint(QPainter *ac_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent)
//{
//  
//  QLineF lv_TheLine = this->line();
//  mv_Terminal_1->setPos(lv_TheLine.p1());
//  mv_Terminal_2->setPos(lv_TheLine.p2());
//
//  mv_Terminal_1->paint(ac_pPainter, ac_pOption, ac_pParent, true);
//  mv_Terminal_2->paint(ac_pPainter, ac_pOption, ac_pParent, true);
//
//  QGraphicsLineItem::paint(ac_pPainter, ac_pOption, ac_pParent);
//}
//
//void CWire::setLine(const QLineF & ac_Line)
//{
//  // Get the points and adjust their position so that they are at the center of the cursor.
//  auto const lv_StartPointF = QPointF(ac_Line.p1().x() - 6, ac_Line.p1().y() - 6);
//  auto const lv_EndPointF   = QPointF(ac_Line.p2().x() - 6, ac_Line.p2().y() - 6);
//
//
//  QLineF lv_LineAdjustedToGrid(CGridUtils::sf_xAdjustPositionToGrid(lv_StartPointF), CGridUtils::sf_xAdjustPositionToGrid(lv_EndPointF));
//  QGraphicsLineItem::setLine(lv_LineAdjustedToGrid);
//}
//
//
//void CWire::keyPressEvent(QKeyEvent *event)
//{
//  if (this->isSelected())
//  {
//    switch (event->key())
//    {
//    case Qt::Key_Delete:
//    case Qt::Key_Backspace:
//      CGridUtils::sc_xTheGrid->removeItem(this);
//      break;
//    }
//  }
//}
//
// Terminal* CWire::mf_OtherTerminal(Terminal * ac_NotThisTerminal)const
//{
//  if (mv_Terminal_1 == ac_NotThisTerminal)
//  {
//    return mv_Terminal_2;
//  }
//  else
//  {
//    return mv_Terminal_1;
//  }
//}