/*
 * SizeConstraints.cpp
 *
 *  Created on: 10 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t SizeConstraints::DESC[] =
        {
            { "",               PT_STRING   },
            { ".min_width",     PT_INT      },
            { ".min_height",    PT_INT      },
            { ".max_width",     PT_INT      },
            { ".max_height",    PT_INT      },
            { NULL,             PT_UNKNOWN  }
        };

        void SizeConstraints::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        SizeConstraints::SizeConstraints(prop::Listener *listener):
            MultiProperty(listener),
            sListener(this)
        {
            sValue.nMinWidth    = -1;
            sValue.nMinHeight   = -1;
            sValue.nMaxWidth    = -1;
            sValue.nMaxHeight   = -1;
        }

        SizeConstraints::~SizeConstraints()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void SizeConstraints::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            ws::size_limit_t &p   = sValue;

            ssize_t v;
            if ((property == vAtoms[P_MIN_WIDTH]) && (pStyle->get_int(vAtoms[P_MIN_WIDTH], &v) == STATUS_OK))
                p.nMinWidth     = lsp_max(v, -1);
            if ((property == vAtoms[P_MIN_HEIGHT]) && (pStyle->get_int(vAtoms[P_MIN_HEIGHT], &v) == STATUS_OK))
                p.nMinHeight    = lsp_max(v, -1);
            if ((property == vAtoms[P_MAX_WIDTH]) && (pStyle->get_int(vAtoms[P_MAX_WIDTH], &v) == STATUS_OK))
                p.nMaxWidth     = lsp_max(v, -1);
            if ((property == vAtoms[P_MAX_HEIGHT]) && (pStyle->get_int(vAtoms[P_MAX_HEIGHT], &v) == STATUS_OK))
                p.nMaxHeight    = lsp_max(v, -1);

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
            {
                ssize_t xv[4];
                size_t n = Property::parse_ints(xv, 4, &s);
                if (n == 4)
                {
                    p.nMinWidth     = lsp_max(xv[0], -1);
                    p.nMinHeight    = lsp_max(xv[1], -1);
                    p.nMaxWidth     = lsp_max(xv[2], -1);
                    p.nMaxHeight    = lsp_max(xv[3], -1);
                }
                else if (n == 2)
                {
                    p.nMinWidth     = -1;
                    p.nMinHeight    = -1;
                    p.nMaxWidth     = lsp_max(xv[0], -1);
                    p.nMaxHeight    = lsp_max(xv[1], -1);
                }
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void SizeConstraints::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    ws::size_limit_t &p       = sValue;

                    // Simple components
                    if (vAtoms[P_MIN_WIDTH] >= 0)
                        pStyle->set_int(vAtoms[P_MIN_WIDTH], p.nMinWidth);
                    if (vAtoms[P_MIN_HEIGHT] >= 0)
                        pStyle->set_int(vAtoms[P_MIN_HEIGHT], p.nMinHeight);
                    if (vAtoms[P_MAX_WIDTH] >= 0)
                        pStyle->set_int(vAtoms[P_MAX_WIDTH], p.nMaxWidth);
                    if (vAtoms[P_MAX_HEIGHT] >= 0)
                        pStyle->set_int(vAtoms[P_MAX_HEIGHT], p.nMaxHeight);

                    // Compound objects
                    LSPString s;
                    if (vAtoms[P_VALUE] >= 0)
                    {
                        if (s.fmt_ascii("%ld %ld %ld %ld", long(p.nMinWidth), long(p.nMinHeight), long(p.nMaxWidth), long(p.nMaxHeight)))
                            pStyle->set_string(vAtoms[P_VALUE], &s);
                    }
                }
                pStyle->end();
            }
            if (pListener != NULL)
                pListener->notify(this);
        }

        void SizeConstraints::get(ssize_t *min_width, ssize_t *min_height, ssize_t *max_width, ssize_t *max_height) const
        {
            *min_width  = sValue.nMinWidth;
            *min_height = sValue.nMinHeight;
            *max_width  = sValue.nMaxWidth;
            *max_height = sValue.nMaxHeight;
        }

        void SizeConstraints::get(ssize_t &min_width, ssize_t &min_height, ssize_t &max_width, ssize_t &max_height) const
        {
            min_width   = sValue.nMinWidth;
            min_height  = sValue.nMinHeight;
            max_width   = sValue.nMaxWidth;
            max_height  = sValue.nMaxHeight;
        }

        ssize_t SizeConstraints::set_min_width(ssize_t value)
        {
            value               = lsp_max(value, -1);
            ssize_t old         = sValue.nMinWidth;
            if (old == value)
                return value;

            sValue.nMinWidth    = value;
            sync();
            return old;
        }

        ssize_t SizeConstraints::set_min_height(ssize_t value)
        {
            value               = lsp_max(value, -1);
            ssize_t old         = sValue.nMinHeight;
            if (old == value)
                return value;

            sValue.nMinHeight   = value;
            sync();
            return old;
        }

        ssize_t SizeConstraints::set_max_width(ssize_t value)
        {
            value               = lsp_max(value, -1);
            ssize_t old         = sValue.nMaxWidth;
            if (old == value)
                return value;

            sValue.nMaxWidth    = value;
            sync();
            return old;
        }

        ssize_t SizeConstraints::set_max_height(ssize_t value)
        {
            value               = lsp_max(value, -1);
            ssize_t old         = sValue.nMaxHeight;
            if (old == value)
                return value;

            sValue.nMaxHeight   = value;
            sync();
            return old;
        }

        void SizeConstraints::set_min(ssize_t width, ssize_t height)
        {
            width               = lsp_max(width, -1);
            height              = lsp_max(height, -1);
            if ((sValue.nMinWidth == width) &&
                (sValue.nMinHeight == height))
                return;

            sValue.nMinWidth    = width;
            sValue.nMinHeight   = height;
            sync();
        }

        void SizeConstraints::set_max(ssize_t width, ssize_t height)
        {
            width               = lsp_max(width, -1);
            height              = lsp_max(height, -1);
            if ((sValue.nMaxWidth == width) &&
                (sValue.nMaxHeight == height))
                return;

            sValue.nMaxWidth    = width;
            sValue.nMaxHeight   = height;
            sync();
        }

        void SizeConstraints::set_width(ssize_t min, ssize_t max)
        {
            min                 = lsp_max(min, -1);
            max                 = lsp_max(max, -1);
            if ((sValue.nMinWidth == min) &&
                (sValue.nMaxWidth == max))
                return;

            sValue.nMinWidth    = min;
            sValue.nMaxWidth    = max;
            sync();
        }

        void SizeConstraints::set_height(ssize_t min, ssize_t max)
        {
            min                 = lsp_max(min, -1);
            max                 = lsp_max(max, -1);
            if ((sValue.nMinHeight == min) &&
                (sValue.nMaxHeight == max))
                return;

            sValue.nMinHeight   = min;
            sValue.nMaxHeight   = max;
            sync();
        }

        void SizeConstraints::set(ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height)
        {
            min_width           = lsp_max(min_width, -1);
            min_height          = lsp_max(min_height, -1);
            max_width           = lsp_max(max_width, -1);
            max_height          = lsp_max(max_height, -1);

            if ((sValue.nMinWidth == min_width) &&
                (sValue.nMinHeight == min_height) &&
                (sValue.nMaxWidth == max_width) &&
                (sValue.nMaxHeight == max_height))
                return;

            sValue.nMinWidth    = min_width;
            sValue.nMinHeight   = min_height;
            sValue.nMaxWidth    = max_width;
            sValue.nMaxHeight   = max_height;
            sync();
        }

        void SizeConstraints::set(const ws::size_limit_t *p)
        {
            ssize_t min_width   = lsp_max(p->nMinWidth, -1);
            ssize_t min_height  = lsp_max(p->nMinHeight, -1);
            ssize_t max_width   = lsp_max(p->nMaxWidth, -1);
            ssize_t max_height  = lsp_max(p->nMaxHeight, -1);

            if ((sValue.nMinWidth == min_width) &&
                (sValue.nMinHeight == min_height) &&
                (sValue.nMaxWidth == max_width) &&
                (sValue.nMaxHeight == max_height))
                return;

            sValue.nMinWidth    = min_width;
            sValue.nMinHeight   = min_height;
            sValue.nMaxWidth    = max_width;
            sValue.nMaxHeight   = max_height;
            sync();
        }

        void SizeConstraints::set(const SizeConstraints *p)
        {
            if ((sValue.nMinWidth == p->sValue.nMinWidth) &&
                (sValue.nMinHeight == p->sValue.nMinHeight) &&
                (sValue.nMaxWidth == p->sValue.nMaxWidth) &&
                (sValue.nMaxHeight == p->sValue.nMaxHeight))
                return;

            sValue.nMinWidth    = p->sValue.nMinWidth;
            sValue.nMinHeight   = p->sValue.nMinHeight;
            sValue.nMaxWidth    = p->sValue.nMaxWidth;
            sValue.nMaxHeight   = p->sValue.nMaxHeight;
            sync();
        }

        void SizeConstraints::compute(ws::size_limit_t *limit, float scale)
        {
            scale               = lsp_max(scale, 0.0f);
            limit->nMinWidth    = (sValue.nMinWidth >= 0) ? sValue.nMinWidth * scale : -1;
            limit->nMinHeight   = (sValue.nMinHeight >= 0) ? sValue.nMinHeight * scale : -1;
            limit->nMaxWidth    = (sValue.nMaxWidth >= 0) ? sValue.nMaxWidth * scale : -1;
            limit->nMaxHeight   = (sValue.nMaxHeight >= 0) ? sValue.nMaxHeight * scale : -1;
        }

        void SizeConstraints::apply(ws::size_limit_t *dst, const ws::size_limit_t *src, float scale)
        {
            // Compute self parameters
            ws::size_limit_t l;
            compute(&l, scale);

            // Compute maximum width & height
            if (src->nMaxWidth >= 0)
                dst->nMaxWidth  = (l.nMaxWidth >= 0) ? lsp_min(src->nMaxWidth, l.nMaxWidth) : src->nMaxWidth;
            else
                dst->nMaxWidth  = l.nMaxWidth;

            if (src->nMaxHeight >= 0)
                dst->nMaxHeight = (l.nMaxHeight >= 0) ? lsp_min(src->nMaxHeight, l.nMaxHeight) : src->nMaxHeight;
            else
                dst->nMaxHeight = l.nMaxHeight;

            // Compute minimum width & height
            if (src->nMinWidth >= 0)
                dst->nMinWidth  = (l.nMinWidth >= 0) ? lsp_max(src->nMinWidth, l.nMinWidth) : src->nMinWidth;
            else
                dst->nMinWidth  = l.nMinWidth;

            if (src->nMinHeight >= 0)
                dst->nMinHeight = (l.nMinHeight >= 0) ? lsp_max(src->nMinHeight, l.nMinHeight) : src->nMinHeight;
            else
                dst->nMinHeight = l.nMinHeight;

            // Maximum width should not be less than minimum width
            if ((dst->nMinWidth >= 0) && (dst->nMaxWidth >= 0))
                dst->nMaxWidth  = lsp_max(dst->nMinWidth, dst->nMaxWidth);

            // Maximum height should not be less than minimum height
            if ((dst->nMinHeight >= 0) && (dst->nMaxHeight >= 0))
                dst->nMaxHeight = lsp_max(dst->nMinHeight, dst->nMaxHeight);
        }

        void SizeConstraints::apply(ws::rectangle_t *rect, float scale)
        {
            scale   = lsp_max(0.0f, scale);

            ws::size_limit_t l;
            l.nMinWidth     = (sValue.nMinWidth >= 0)  ? sValue.nMinWidth  * scale : -1;
            l.nMinHeight    = (sValue.nMinHeight >= 0) ? sValue.nMinHeight * scale : -1;
            l.nMaxWidth     = (sValue.nMaxWidth >= 0)  ? sValue.nMaxWidth  * scale : -1;
            l.nMaxHeight    = (sValue.nMaxHeight >= 0) ? sValue.nMaxHeight * scale : -1;

            apply(rect, &l);
        }

        void SizeConstraints::apply(ws::rectangle_t *dst, const ws::size_limit_t *sc)
        {
            if ((sc->nMaxWidth >= 0) && (dst->nWidth > sc->nMaxWidth))
                dst->nWidth     = sc->nMaxWidth;
            if ((sc->nMaxHeight >= 0) && (dst->nHeight > sc->nMaxHeight))
                dst->nHeight    = sc->nMaxHeight;
            if ((sc->nMinWidth >= 0) && (dst->nWidth < sc->nMinWidth))
                dst->nWidth     = sc->nMinWidth;
            if ((sc->nMinHeight >= 0) && (dst->nHeight < sc->nMinHeight))
                dst->nHeight    = sc->nMinHeight;
        }

        void SizeConstraints::apply(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::size_limit_t *sc)
        {
            dst->nLeft      = src->nLeft;
            dst->nTop       = src->nTop;
            dst->nWidth     = ((sc->nMaxWidth >= 0) && (src->nWidth > sc->nMaxWidth))    ? sc->nMaxWidth  : src->nWidth;
            dst->nHeight    = ((sc->nMaxHeight >= 0) && (src->nHeight > sc->nMaxHeight)) ? sc->nMaxHeight : src->nHeight;

            if ((sc->nMinWidth >= 0) && (dst->nWidth < sc->nMinWidth))
                dst->nWidth     = sc->nMinWidth;
            if ((sc->nMinHeight >= 0) && (dst->nHeight < sc->nMinHeight))
                dst->nHeight    = sc->nMinHeight;
        }

        namespace prop
        {
            status_t SizeConstraints::init(Style *style, ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_int(vAtoms[P_MIN_WIDTH], min_width);
                    style->create_int(vAtoms[P_MIN_HEIGHT], min_height);
                    style->create_int(vAtoms[P_MAX_WIDTH], max_width);
                    style->create_int(vAtoms[P_MAX_HEIGHT], max_height);
                }
                style->end();
                return STATUS_OK;
            }

            status_t SizeConstraints::init(Style *style, const ws::size_limit_t *p)
            {
                if ((pStyle == NULL) || (p == NULL))
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_int(vAtoms[P_MIN_WIDTH], p->nMinWidth);
                    style->create_int(vAtoms[P_MIN_HEIGHT], p->nMinHeight);
                    style->create_int(vAtoms[P_MAX_WIDTH], p->nMaxWidth);
                    style->create_int(vAtoms[P_MAX_HEIGHT], p->nMaxHeight);
                }
                style->end();
                return STATUS_OK;
            }
        }
    } /* namespace tk */
} /* namespace lsp */


