#include "inc/bitmap.h"

static unsigned roundup(unsigned val, unsigned upto)
{
  return (val + (upto - 1)) & ~(upto - 1);
}


Bitmap::Bitmap(unsigned x, unsigned y, unsigned z) : dimx(roundup(x,8)), dimy(roundup(y,8)), dimz(roundup(z,8))
{
  bits = new char[(dimx*dimy*dimz)/8];
}

Bitmap::~Bitmap()
{
  delete bits;
}

unsigned Bitmap::bitIndex(unsigned x, unsigned y, unsigned z)
{
  return (z*dimx*dimy) + (y*dimx) + x;
}

bool Bitmap::getBit(unsigned x, unsigned y, unsigned z)
{
  unsigned index = bitIndex(x, y, z);
  return 
}
