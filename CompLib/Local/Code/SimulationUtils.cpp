#include <SimulationUtils.h>

QTime SimulationUtils::SimulationTime = QTime::currentTime();

std::function<float*(int&)> SimulationUtils::TheScopeValue = nullptr;