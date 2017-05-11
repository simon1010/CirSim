#pragma once
#ifndef CIRCUITCOMPOSER_H
#define CIRCUITCOMPOSER_H

#include <QtWidgets\qgraphicsitem.h>

#include <DSPatch.h>

class CCircuitComposer
{
public:

  CCircuitComposer(const QList<QGraphicsItem*>& av_QList_AllElements);

private:
  void mf_InterpretQGrapghicsItems(const QList<QGraphicsItem*>& av_QList_AllElements);

};

#endif // !CIRCUITCOMPOSER_H
