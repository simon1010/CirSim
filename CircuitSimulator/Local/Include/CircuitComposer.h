#pragma once
#ifndef CIRCUITCOMPOSER_H
#define CIRCUITCOMPOSER_H

#include <QtWidgets\qgraphicsitem.h>
#include <Terminal.h>
#include <Node.h>
#include <DSPatch.h>

class CCircuitComposer
{
public:

  CCircuitComposer(const QList<QGraphicsItem*>& av_QList_AllElements);

private:
  void mf_InterpretQGrapghicsItems(const QList<QGraphicsItem*>& av_QList_AllElements);
  DspCircuit mv_TheCircuit;
};

#endif // !CIRCUITCOMPOSER_H
