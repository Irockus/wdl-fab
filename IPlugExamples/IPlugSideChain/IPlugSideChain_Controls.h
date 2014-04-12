class IPeakMeterVert : public IControl
{
public:

  IPeakMeterVert(IPlugBase* pPlug, IRECT pR)
    : IControl(pPlug, pR)
  {
    mColor = COLOR_BLUE;
  }

  ~IPeakMeterVert() {}

  bool Draw(IGraphics* pGraphics)
  {
    //IRECT(mRECT.L, mRECT.T, mRECT.W , mRECT.T + (mValue * mRECT.H));
    pGraphics->FillIRect(&COLOR_BLACK, &mRECT);

    //pGraphics->FillIRect(&COLOR_BLUE, &mRECT);

    IRECT filledBit = IRECT(mRECT.L, mRECT.T, mRECT.R , mRECT.B - (mValue * mRECT.H()));
    pGraphics->FillIRect(&mColor, &filledBit);
    return true;
  }

  bool IsDirty() { return true;}

protected:
  IColor mColor;
};

class IPeakMeterHoriz : public IPeakMeterVert
{
public:

  bool Draw(IGraphics* pGraphics)
  {
    pGraphics->FillIRect(&COLOR_BLUE, &mRECT);
    IRECT filledBit = IRECT(mRECT.L, mRECT.T, mRECT.L + (mValue * mRECT.W() ) , mRECT.B );
    pGraphics->FillIRect(&mColor, &filledBit);
    return true;
  }
};
