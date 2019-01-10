#pragma once

#include "LGObject.h"

class SceneParser : public LGObject
{
    LG_DECLARE_DYNCREATE(SceneParser);

public:
    virtual BOOL parse(CString fileName,LGAPIInterface&api);
};
