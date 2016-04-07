#include "Algos.h"

namespace Algos
{

QString strOctets(double val)
{
    const QString ranges[] = {"o","Ko","Mo","Go"};
    int i = 0;
    while(val > 1024.0)
    {
        val /= 1024.0;
        i++;
    }
    return QString::number(val, 'g', 3) + " " + ranges[i];
}


}
