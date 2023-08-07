/*
cartotype_qt_map_renderer.h

Copyright (C) 2017-2023 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <QOpenGLFunctions>
#include "cartotype.h"

namespace CartoTypeCore
{

class CVectorTileServer;

/** A graphics-accelerated map renderer for use with Qt. */
class CQtMapRenderer
    {
    public:
    CQtMapRenderer(Framework& aFramework);
    static std::unique_ptr<CQtMapRenderer> New(Result& aError,Framework& aFramework);
    void Draw();

    private:
    std::shared_ptr<CVectorTileServer> m_vector_tile_server;
    };

}
