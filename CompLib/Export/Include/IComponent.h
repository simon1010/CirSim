#pragma once

#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QSet>
#include <QObject>
#include <QMenu>

#include <ComponentDialog.h>
#include <Terminal.h>
#include <SimulationUtils.h>

#include <IDispatchComponent.h>
#include <qjsonobject.h>

class Terminal;
// not really an interface but it's the first class to implement QGraphisItem,
// all Elements are to derive from this
// should handle basic grid functionality, so other objects should not bother
#include <complib_global.h>

class COMPLIB_EXPORT IComponent : public QObject, public QGraphicsItem, public IDispatchComponent
{
  Q_OBJECT
public:
  IComponent(QGraphicsScene *ac_pScene, QGraphicsView *parent = 0);
  IComponent(QPointF ac_Position, QGraphicsScene *ac_pScene, QGraphicsView *parent = 0);
  ~IComponent();


  //                        present value
  //               valueName   |
  //                 |         |
  typedef std::map<QString, double> DialogSettingsMap;


  /* Inherited public virtual methods */
  void update(const QRectF & rect = QRectF());
  virtual void paint(QPainter *lc_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent);
  virtual void mf_DerivedPaint(QPainter *lc_pPainter, const QStyleOptionGraphicsItem *ac_pOption, QWidget *ac_pParent) = 0;
  
  virtual const QString mf_szGetCompName() const;
  const QString mf_szGetCompID() const;
  void mf_AddTerminal(Terminal *ac_Terminal);
  Terminal* mf_OtherTerminal(Terminal * ac_NotThisTerminal) const;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

  virtual void mf_Save(QJsonObject &json) = 0;
  virtual void mf_Load(QJsonObject &json);

protected:
  // Offers basic snap-to-grid functionality, probably not compatible with all elements
  virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  void mousePressEvent(QGraphicsSceneMouseEvent  *ac_pMouseEvent);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent  *ac_pMouseEvent);

  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *ac_Event);

  virtual DialogSettingsMap mf_GetDialogSettingsMap();
  virtual void mf_SetDialogSettingsMap(DialogSettingsMap);
  virtual void mf_ShowSettingsDialog();

  virtual QString mf_ToolTipGetType() = 0;
  virtual QString mf_ToolTipGetValue() = 0;
  virtual QString mf_ToolTipGetUnit() = 0;

  virtual SAMPLE *mf_dfGetVoltage(int& av_nSamplesAvailable) = 0;

  virtual void mf_PreDestroy();
  
  QDialog *mv_SettingsDialog;

  QString mv_sCompName;
  QString mv_sCompID;
  static unsigned int sv_nCompNumber;

  QSet<Terminal *> mv_Terminals;
  QGraphicsView *mv_pParent;

  virtual const int mf_nGetInstanceNumber() const = 0;

private:

  bool mv_bHasKeyboard;
  void IComponent::keyPressEvent(QKeyEvent *event);

private slots:

void ms_xSliderValueChanged(int Val);
void ms_xViewInScope();
void ms_xPropertiesDialog();
void ms_xRemove();
};

#endif