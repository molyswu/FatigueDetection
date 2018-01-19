#include "core.h"

#include <stdlib.h>
#include "time.h"
TxPoint txPoint(int x, int y)
{
	TxPoint p;
	p.x=x;
	p.y=y;
	return p;
}

TxPoint2f txPoint2f(float x, float y)
{
	TxPoint2f p;
	p.x=x;
	p.y=y;
	return p;
}