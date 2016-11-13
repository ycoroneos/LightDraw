#pragma once

class Bitmap
{
  public:
  Bitmap(unsigned x, unsigned y, unsigned z);
  ~Bitmap();
  unsigned bitIndex(unsigned x, unsigned y, unsigned z);
  bool getBit(unsigned x, unsigned y, unsigned z);
  void setBit(unsigned x, unsigned y, unsigned z);
  void clearBit(unsigned x, unsigned y, unsigned z);
  private:
  unsigned dimx, dimy, dimz;
  char *bits;
};

