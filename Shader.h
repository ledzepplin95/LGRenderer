#pragma once

#include "renderobject.h"

class Shader : public RenderObject 
{
   LG_DECLARE_DYNCREATE(Shader);

public:
   virtual Color getRadiance(ShadingState&state)const;
   virtual void scatterPhoton(ShadingState&state,Color&power)const;
   BOOL operator ==(const Shader&s) const;
   BOOL operator !=(const Shader&s)const;
};