#pragma once
#ifndef SIMULATIONUTILS_H
#define SIMULATIONUTILS_H

#include <QTime>
#include <functional>

typedef float SAMPLE;

#include <complib_global.h>

class COMPLIB_EXPORT SimulationUtils
{
public:

  // Global time source
  static QTime SimulationTime;

  /* funtion pointer, serves as a source of data for the scope */
  //typedef double(IComponent::*ScopeValue)();
  //static ScopeValue TheScopeValue;

  static std::function<SAMPLE*(int&)> TheScopeValue;
  static int AvailableSamples;

  // http://doc.qt.io/qt-4.8/qgraphicsitem.html#type
  enum ElectronicElements
  {
    Element_Terminal = 65536, 
    Element_Resistor,
    Element_DC_Source,
    Element_Wire,
    Element_Ground,
    Element_ProgrammableVoltageSource,
    Element_Capacitor,
    Element_Microphone,
    
    Element_Total
  }; 

};

#endif