#include <STGRectangle.h>
#include <Grid.h>

CSTGRegctangle::CSTGRegctangle(QRect &ac_xRect, QGraphicsItem *ac_pParent, CGrid* ac_pTheGrid) : 
QGraphicsRectItem(mf_xAdjustRectToGrid(ac_xRect, ac_pTheGrid->mf_nGetGridSize()), ac_pParent),
mv_xTheGrid(ac_pTheGrid)
{
  setFlags(QGraphicsItem::ItemIsSelectable |
    QGraphicsItem::ItemIsMovable |
    QGraphicsItem::ItemSendsGeometryChanges);
}

CSTGRegctangle::CSTGRegctangle(QGraphicsRectItem &ac_xCopyFrom) :
QGraphicsRectItem(ac_xCopyFrom.rect(), ac_xCopyFrom.parentItem()),
mv_xTheGrid(nullptr)
{
  _ASSERT(false);
}


QVariant CSTGRegctangle::itemChange(GraphicsItemChange change, const QVariant &value)
{
  if (change == ItemPositionChange && scene())
  {
    QPointF newPos = value.toPointF();
    if (QApplication::mouseButtons() == Qt::LeftButton && (nullptr != mv_xTheGrid))
    {
      const unsigned lc_nGridSize = mv_xTheGrid->mf_nGetGridSize();
      return mf_xAdjustPositionToGrid(lc_nGridSize, newPos);
    }
    else
      return newPos;
  }
  else
    return QGraphicsItem::itemChange(change, value);
}

QPointF CSTGRegctangle::mf_xAdjustPositionToGrid(const unsigned ac_nGridSize, const QPointF &ac_nPosition)
{
  int x = ac_nPosition.x();
  int y = ac_nPosition.y();

  qreal xV = round( ((x % 5) == 0 ? x - 1 : x ) / ac_nGridSize)*ac_nGridSize;
  qreal yV = round( ((y % 5) == 0 ? y - 1 : y )/ ac_nGridSize)*ac_nGridSize;
  return QPointF(xV, yV);
 /* qreal xV = floor(ac_nPosition.x() / ac_nGridSize)*ac_nGridSize;
  qreal yV = floor(ac_nPosition.y() / ac_nGridSize)*ac_nGridSize;
  return QPointF(xV, yV);
*/
}

//int RoundUp(int toRound)
//{
//  if (toRound % 10 == 0)
//    return toRound;
//  return (10 - toRound % 10) + toRound;
//}
//
//int RoundDown(int toRound)
//{
//  if (toRound % 10 == 0)
//    return toRound;
//  return toRound - toRound % 10;
//}

QRect CSTGRegctangle::mf_xAdjustRectToGrid(const QRect &ac_xRectangle, const unsigned ac_nGridSize)
{
  QPoint lv_xTopLeft = ac_xRectangle.topLeft();
  QPoint lv_xBotRigt = ac_xRectangle.bottomRight();
  
  QPoint lv_xNewTopLeft = mf_xAdjustPositionToGrid(ac_nGridSize, lv_xTopLeft).toPoint();
  QPoint lv_xNewBotRigt = mf_xAdjustPositionToGrid(ac_nGridSize, lv_xBotRigt).toPoint();

  //int TopX = lv_xNewTopLeft.x(), TopY = lv_xNewTopLeft.y(),
  //    BotX = lv_xBotRigt.x(), BotY = lv_xNewBotRigt.y();
  //
  //// move down if right in middle
  //TopX = RoundUp(TopX);
  //TopY = RoundUp(TopY);
  //if (TopX == TopY)
  //{
  //  TopY += ac_nGridSize;
  //}
  //BotX = RoundUp(BotX);
  //BotY = RoundUp(BotY);
  //if (BotX == BotY)
  //{
  //  BotY += ac_nGridSize;
  //}

  //QRect lv_xRectangle(QPoint(TopX, TopY), QPoint(BotX, BotY));
  return QRect(lv_xNewTopLeft, lv_xNewBotRigt);

  // PostCondition: Must be multiple of eachother so that we have a rectangle
  //_ASSERT(((lv_xTopLeft.x - lv_xBotRigt.x) % lc_nGridSize == 0) && ((lv_xTopLeft.y - lv_xBotRigt.y) % lc_nGridSize == 0));
  
}