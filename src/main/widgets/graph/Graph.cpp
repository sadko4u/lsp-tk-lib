/*
 * Graph.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Graph::metadata         =   { "Graph", &WidgetContainer::metadata };

        Graph::Graph(Display *dpy):
            WidgetContainer(dpy),
            vItems(&sProperties, &sIListener),
            sConstraints(&sProperties),
            sBorder(&sProperties),
            sBorderRadius(&sProperties),
            sGlass(&sProperties),
            sColor(&sProperties),
            sBorderColor(&sProperties),
            sGlassColor(&sProperties),
            sIPadding(&sProperties)
        {
            pGlass              = NULL;

            sCanvas.nLeft       = 0;
            sCanvas.nTop        = 0;
            sCanvas.nWidth      = 0;
            sCanvas.nHeight     = 0;

            pClass              = &metadata;
        }

        Graph::~Graph()
        {
            do_destroy();
        }

        void Graph::destroy()
        {
            WidgetContainer::destroy();
            do_destroy();
        }

        void Graph::do_destroy()
        {
            // Unlink all items
            size_t n            = vItems.size();
            for (size_t i=0; i<n; ++i)
            {
                GraphItem *item     = vItems.uget(i);
                if (item == NULL)
                    continue;

                unlink_widget(item);
            }

            // Destroy glass
            if (pGlass != NULL)
            {
                pGlass->destroy();
                delete pGlass;
                pGlass = NULL;
            }

            vItems.flush();
        }

        status_t Graph::init()
        {
            status_t result = WidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            // Init listener
            sIListener.bind_all(this, on_add_item, on_remove_item);

            // Init style
            sConstraints.bind("size.constraints", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sGlass.bind("glass.visibility", &sStyle);
            sColor.bind("color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sGlassColor.bind("glass.color", &sStyle);
            sIPadding.bind("padding.internal", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sConstraints.init(sclass);
                sBorder.init(sclass, 4);
                sBorderRadius.init(sclass, 12);
                sGlass.init(sclass, true);
                sColor.init(sclass, "#000000");
                sBorderColor.init(sclass, "#000000");
                sGlassColor.init(sclass, "#ffffff");
                sIPadding.init(sclass, 1);
            }

            return STATUS_OK;
        }

        void Graph::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);
            if (vItems.is(prop))
                query_draw();
        }

        void Graph::size_request(ws::size_limit_t *r)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            sConstraints.compute(r, scaling);
            sIPadding.add(r, scaling);

            float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius
            float bw        = lsp_max(0.0f, sBorder.get() * scaling);       // border size
            float ir        = lsp_max(0.0f, xr - bw);                       // internal radius
            float bp        = (1.0f - M_SQRT1_2) * ir;                      // padding to not to cross internal radius
            ssize_t padding = ceilf(bp + bw);
            ssize_t wh      = lsp_max(padding * 2, xr * 2);                 // minimum possible width and height

            // Append padding to the size limit
            r->nMinWidth    = (r->nMinWidth  >= 0) ? r->nMinWidth  + padding * 2 : padding * 2;
            r->nMinHeight   = (r->nMinHeight >= 0) ? r->nMinHeight + padding * 2 : padding * 2;
            r->nMaxWidth    = (r->nMaxWidth  >= 0) ? r->nMaxWidth  + padding * 2 : -1;
            r->nMaxHeight   = (r->nMaxHeight >= 0) ? r->nMaxHeight + padding * 2 : -1;

            // Apply radius settings
            r->nMinWidth    = (r->nMinWidth  >= 0) ? lsp_max(r->nMinWidth,  wh)  : wh;
            r->nMinHeight   = (r->nMinHeight >= 0) ? lsp_max(r->nMinHeight, wh)  : wh;
            if ((r->nMaxWidth  >= 0) && (r->nMaxWidth  < r->nMinWidth))
                r->nMaxWidth    = r->nMinWidth;
            if ((r->nMaxHeight >= 0) && (r->nMaxHeight < r->nMinHeight))
                r->nMaxHeight   = r->nMinHeight;
        }

        void Graph::realize(const ws::rectangle_t *r)
        {
            // Call parent class to realize
            WidgetContainer::realize(r);

            // Compute the size of area
            float scaling   = lsp_max(0.0f, sScaling.get());
            float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius
            float bw        = lsp_max(0.0f, sBorder.get() * scaling);       // border size
            float ir        = lsp_max(0.0f, xr - bw);                       // internal radius
            ssize_t padding = (1.0f - M_SQRT1_2) * ir + bw;                 // padding of internal area

            sCanvas.nLeft   = r->nLeft   + padding;
            sCanvas.nTop    = r->nTop    + padding;
            sCanvas.nWidth  = r->nWidth  - padding*2;
            sCanvas.nHeight = r->nHeight - padding*2;
            sIPadding.enter(&sCanvas, scaling);
        }

        void Graph::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            float scaling   = lsp_max(0.0f, sScaling.get());
            float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius
            float bw        = lsp_max(0.0f, sBorder.get() * scaling);       // border size
            float ir        = lsp_max(0.0f, xr - bw);                       // internal radius
            float bright    = sBrightness.get();

            lsp_trace("xr = %f, size={%d, %d, %d, %d}", xr,
                    int(sSize.nLeft), int(sSize.nTop), int(sSize.nWidth), int(sSize.nHeight)
                );

            // Prepare palette
            ws::ISurface *cv;
            lsp::Color color(sColor);
            lsp::Color bg_color(sBgColor);
            color.scale_lightness(bright);

            s->clip_begin(area);
            {
                // Draw widget background
                s->fill_rect(bg_color, &sSize);

                bool aa = s->set_antialiasing(true);
                s->fill_round_rect(color, SURFMASK_ALL_CORNER, xr, &sSize);

                // Get surface of widget
                cv  = get_surface(s, sCanvas.nWidth, sCanvas.nHeight);
                if (cv != NULL)
                    s->draw(cv, sSize.nLeft + ir, sSize.nTop + ir);

                // Draw the glass and the border
                color.copy(sGlassColor);
                bg_color.copy(sColor);
                color.scale_lightness(bright);
                bg_color.scale_lightness(bright);

                cv = create_border_glass(&pGlass, s,
                        color, bg_color,
                        SURFMASK_ALL_CORNER, bw, xr,
                        sSize.nWidth, sSize.nHeight
                    );
                if (cv != NULL)
                    s->draw(cv, sSize.nLeft, sSize.nTop);

                s->set_antialiasing(aa);
            }
            s->clip_end();
        }

        void Graph::draw(ws::ISurface *s)
        {
            // Clear canvas
            lsp::Color c(sColor);
            c.scale_lightness(sBrightness.get());
            s->clear(&c);

            // Draw all objects
            ws::rectangle_t r;
            r.nLeft   = 0;
            r.nTop    = 0;
            r.nWidth  = s->width();
            r.nHeight = s->height();

            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                GraphItem *gi = vItems.get(i);
                if ((gi == NULL) || (!gi->visibility()->get()))
                    continue;

                gi->render(s, &r, true);
                gi->commit_redraw();
            }

        }

        Widget *Graph::find_widget(ssize_t x, ssize_t y)
        {
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                GraphItem *gi = vItems.get(i);
                if ((gi == NULL) || (!gi->visibility()->get()))
                    continue;

                if (gi->inside(x, y))
                    return gi;
            }
            return NULL;
        }

        void Graph::on_add_item(void *obj, Property *prop, Widget *w)
        {
            GraphItem *item = widget_cast<GraphItem>(w);
            if (item == NULL)
                return;

            Graph *_this = widget_ptrcast<Graph>(obj);
            if (_this == NULL)
                return;

            item->set_parent(_this);
            _this->query_draw();
        }

        void Graph::on_remove_item(void *obj, Property *prop, Widget *w)
        {
            GraphItem *item = widget_cast<GraphItem>(w);
            if (item == NULL)
                return;

            Graph *_this = widget_ptrcast<Graph>(obj);
            if (_this == NULL)
                return;

            _this->unlink_widget(item);
            _this->query_draw();
        }

        bool Graph::origin(size_t index, float *x, float *y)
        {
            return origin(vOrigins.get(index), x, y);
        }

        bool Graph::origin(GraphOrigin *o, float *x, float *y)
        {
            if (o == NULL)
            {
                *x      = 0.0f;
                *y      = 0.0f;
                return false;
            }

            float c_width   = sCanvas.nWidth;
            float c_height  = sCanvas.nHeight;

            *x  = float(sIPadding.left()) + (1.0f + o->left()->get()) * c_width  * 0.5f;
            *y  = float(sIPadding.top())  + (1.0f - o->top()->get() ) * c_height * 0.5f;

            return true;
        }

        status_t Graph::add(Widget *child)
        {
            GraphItem *item     = widget_cast<GraphItem>(child);
            return (item != NULL) ? vItems.add(item) : STATUS_BAD_TYPE;
        }

        status_t Graph::remove(Widget *child)
        {
            GraphItem *item     = widget_cast<GraphItem>(child);
            return (item != NULL) ? vItems.premove(item) : STATUS_BAD_TYPE;
        }

        status_t Graph::remove_all()
        {
            vItems.clear();
            return STATUS_OK;
        }
    }
}
