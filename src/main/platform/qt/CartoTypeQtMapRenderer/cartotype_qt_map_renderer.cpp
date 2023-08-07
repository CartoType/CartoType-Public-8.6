/*
cartotype_qt_map_renderer.cpp

Copyright (C) 2017-2023 CartoType Ltd.
See www.cartotype.com for more information.
*/

#include "cartotype_qt_map_renderer.h"
#include "cartotype_vector_tile.h"

namespace CartoTypeCore
{

QOpenGLFunctions TheOpenGLFunctions;

/** Creates an OpenGL ES 2.0 map renderer for use with Qt. Call this from QOpenGLWidget::initializeGL. */
CQtMapRenderer::CQtMapRenderer(Framework& aFramework)
    {
    m_vector_tile_server = CreateOpenGLESVectorTileServer(aFramework);
    TheOpenGLFunctions.initializeOpenGLFunctions();
    StartOpenGLESVectorTileServer(*m_vector_tile_server);
    }

/** Creates an OpenGL ES 2.0 map renderer for use with Qt. Returns an error code instead of possibly throwing an exception. Call this from QOpenGLWidget::initializeGL. */
std::unique_ptr<CQtMapRenderer> CQtMapRenderer::New(Result& aError,Framework& aFramework)
    {
    auto p = std::unique_ptr<CQtMapRenderer>();
    try
        {
        p = std::make_unique<CQtMapRenderer>(aFramework);
        }
    catch (Result e)
        {
        aError = e;
        p = nullptr;
        }
    return p;
    }

/** Draws the map. Call this from QOpenGLWidget::paintGL. */
void CQtMapRenderer::Draw()
    {
    m_vector_tile_server->Draw();
    }

}
