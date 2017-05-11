#pragma once
#ifndef SIMULATIONUTILS_H
#define SIMULATIONUTILS_H

#include <QTime>
#include <functional>
#include <string>
#include <stdio.h>
#include <memory>
typedef float SAMPLE;

#include <complib_global.h>
#include <QtWidgets\qgraphicsitem.h>

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

  static void sf_PrintElementsToFile(const QList<QGraphicsItem*>& ac_QList_AllElements, const char * ac_LogFile);

  template<typename ... Args>
  static std::string string_format(const std::string& format, Args ... args)
  {
    size_t size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
  }
};

#endif