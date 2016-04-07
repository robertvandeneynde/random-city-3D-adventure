#include "Parametres.h"
#include <vector>

const int Parametres::DT_INT_MS = 30;
const double Parametres::DT = Parametres::DT_INT_MS / 1000.0;
const double Parametres::DT_2 = Parametres::DT * Parametres::DT;
const double Parametres::FPS = 1.0 / Parametres::DT;
const double Parametres::FPS_2 = Parametres::FPS / Parametres::FPS;
