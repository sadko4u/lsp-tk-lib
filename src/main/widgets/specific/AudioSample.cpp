/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 28 сент. 2020 г.
 *
 * lsp-tk-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-tk-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-tk-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/tk/helpers/draw.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t AudioSample::metadata           = { "AudioSample", &WidgetContainer::metadata };

        AudioSample::AudioSample(Display *dpy):
            WidgetContainer(dpy),
            vChannels(&sProperties, &sIListener),
            sWaveBorder(&sProperties),
            sFadeInBorder(&sProperties),
            sFadeOutBorder(&sProperties),
            sLineWidth(&sProperties),
            sConstraints(&sProperties),
            sActive(&sProperties),
            sSGroups(&sProperties),
            sMainText(&sProperties),
            sMainLayout(&sProperties),
            sMainFont(&sProperties),
            sMainColor(&sProperties),
            sMainVisibility(&sProperties),
            sLabelFont(&sProperties),
            sLabelBgColor(&sProperties),
            sLabelRadius(&sProperties),
            sBorder(&sProperties),
            sBorderRadius(&sProperties),
            sGlass(&sProperties),
            sColor(&sProperties),
            sBorderColor(&sProperties),
            sGlassColor(&sProperties),
            sIPadding(&sProperties)
        {
            for (size_t i=0; i<LABELS; ++i)
            {
                sLabel[i].listener(&sProperties);
                sLabelColor[i].listener(&sProperties);
                sLabelLayout[i].listener(&sProperties);
                sLabelVisibility[i].listener(&sProperties);
            }

            nBMask              = 0;
            nXFlags             = 0;

            sGraph.nLeft        = 0;
            sGraph.nTop         = 0;
            sGraph.nWidth       = 0;
            sGraph.nHeight      = 0;

            pGlass              = NULL;

            pClass              = &metadata;
        }

        AudioSample::~AudioSample()
        {
            do_destroy();
        }

        void AudioSample::destroy()
        {
            WidgetContainer::destroy();
            do_destroy();
        }

        void AudioSample::drop_glass()
        {
            if (pGlass != NULL)
            {
                pGlass->destroy();
                delete pGlass;
                pGlass      = NULL;
            }
        }

        void AudioSample::do_destroy()
        {
            // Unlink all items
            for (size_t i=0, n=vChannels.size(); i<n; ++i)
            {
                AudioChannel *item   = vChannels.uget(i);
                if (item == NULL)
                    continue;

                unlink_widget(item);
            }

            // Drop glass
            drop_glass();

            // Flush containers
            vChannels.flush();
            vVisible.flush();
        }

        status_t AudioSample::init()
        {
            status_t res = WidgetContainer::init();
            if (res != STATUS_OK)
                return res;

            // Init listener
            sIListener.bind_all(this, on_add_item, on_remove_item);

            // Bind properties
            sWaveBorder.bind("wave.border", &sStyle);
            sFadeInBorder.bind("fade_in.border", &sStyle);
            sFadeOutBorder.bind("fade_out.border", &sStyle);
            sLineWidth.bind("line.width", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);
            sActive.bind("active", &sStyle);
            sSGroups.bind("stereo_groups", &sStyle);
            sMainText.bind(&sStyle, pDisplay->dictionary());
            sMainLayout.bind("main.layout", &sStyle);
            sMainFont.bind("main.font", &sStyle);
            sMainVisibility.bind("main.visibility", &sStyle);
            sLabelFont.bind("label.font", &sStyle);
            sLabelBgColor.bind("label.bg.color", &sStyle);
            sLabelRadius.bind("label.radius", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sGlass.bind("glass", &sStyle);
            sColor.bind("color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sGlassColor.bind("glass.color", &sStyle);
            sIPadding.bind("padding.internal", &sStyle);

            for (size_t i=0; i<LABELS; ++i)
            {
                LSPString id;

                sLabel[i].bind(&sStyle, pDisplay->dictionary());
                id.fmt_ascii("label.%d.text.color", int(i));
                sLabelColor[i].bind(&id, &sStyle);
                id.fmt_ascii("label.%d.layout", int(i));
                sLabelLayout[i].bind(&id, &sStyle);
                id.fmt_ascii("label.%d.visibility", int(i));
                sLabelVisibility[i].bind(&id, &sStyle);
            }

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sWaveBorder.init(sclass, 1);
                sFadeInBorder.init(sclass, 1);
                sFadeOutBorder.init(sclass, 1);
                sLineWidth.init(sclass, 1);
                sConstraints.init(sclass);
                sActive.init(sclass, false);
                sSGroups.init(sclass, false);

                sMainLayout.init(sclass, 0.0f, 0.0f);
                sMainFont.init(sclass, 16.0f);
                sMainColor.init(sclass, "#00ff00");
                sMainVisibility.init(sclass, false);
                sLabelFont.init(sclass, 10.0f);
                sLabelBgColor.init(sclass, "#88000000");
                sLabelRadius.init(sclass, 2);

                sBorder.init(sclass, 4);
                sBorderRadius.init(sclass, 12);
                sGlass.init(sclass, true);
                sColor.init(sclass, "#000000");
                sBorderColor.init(sclass, "#000000");
                sGlassColor.init(sclass, "#ffffff");
                sIPadding.init(sclass, 1);

                for (size_t i=0; i<LABELS; ++i)
                {
                    sLabelColor[i].init(sclass, "#ffffff");
                    sLabelLayout[i].init(sclass, 0.0f, 0.0f);
                    sLabelVisibility[i].init(sclass, false);
                }
            }

            return STATUS_OK;
        }

        void AudioSample::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (vChannels.is(prop))
                query_resize();
            if (sWaveBorder.is(prop))
                query_resize();
            if (sFadeInBorder.is(prop))
                query_draw();
            if (sFadeOutBorder.is(prop))
                query_draw();
            if (sLineWidth.is(prop))
                query_draw();
            if (sConstraints.is(prop))
                query_resize();
            if (sSGroups.is(prop))
                query_resize();

            if ((sMainText.is(prop)) && (sMainVisibility.get()))
                query_draw();
            if ((sMainLayout.is(prop)) && (sMainVisibility.get()))
                query_draw();
            if ((sMainFont.is(prop)) && (sMainVisibility.get()))
                query_draw();
            if (sMainVisibility.is(prop))
                query_draw();
            if (sLabelFont.is(prop))
                query_draw();
            if (sLabelBgColor.is(prop))
                query_draw();
            if (sLabelRadius.is(prop))
                query_draw();

            if (sBorder.is(prop))
                query_resize();
            if (sBorderRadius.is(prop))
                query_resize();
            if (sGlass.is(prop))
                query_draw();
            if (sColor.is(prop))
                query_draw();
            if (sBorderColor.is(prop))
                query_draw();
            if (sGlassColor.is(prop))
                query_draw();
            if (sIPadding.is(prop))
                query_resize();

            for (size_t i=0; i<LABELS; ++i)
            {
                if (sLabelColor[i].is(prop))
                    query_draw();
                if (sLabelLayout[i].is(prop))
                    query_draw();
                if (sLabelVisibility[i].is(prop))
                    query_draw();
            }
        }

        void AudioSample::size_request(ws::size_limit_t *r)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            bool sgroups        = sSGroups.get();

            lltl::parray<AudioChannel> channels;
            get_visible_items(&channels);

            // Estimate the size of area for drawing samples
            ws::size_limit_t sl;
            r->nMinWidth        = 0;
            r->nMinHeight       = 0;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
            r->nPreWidth        = -1;
            r->nPreHeight       = -1;

            for (size_t i=0, n=channels.size(); i<n; ++i)
            {
                AudioChannel *c     = channels.uget(i);
                c->constraints()->compute(&sl, scaling);
                ssize_t h           = lsp_max(0, sl.nMinHeight);
                r->nMinWidth        = lsp_max(r->nMinWidth, sl.nMinWidth);
                r->nMinHeight      += (sgroups) ? (h >> 1) : h;
            }

            // Add padding to the rectangle
            sIPadding.add(r, scaling);

            // Compute additional space around the sample area
            float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius
            float bw        = lsp_max(0.0f, sBorder.get() * scaling);       // border size
            float ir        = lsp_max(0.0f, xr - bw);                       // internal radius
            float bp        = (1.0f - M_SQRT1_2) * ir;                      // padding to not to cross internal radius
            ssize_t padding = ceilf(bp + bw);
            ssize_t wh      = lsp_max(padding * 2, xr * 2);                 // minimum possible width and height

            r->nMinWidth    = lsp_max(r->nMinWidth  + padding * 2, wh);
            r->nMinHeight   = lsp_max(r->nMinHeight + padding * 2, wh);

            // Apply size constraints
            sConstraints.apply(r, scaling);
        }

        void AudioSample::realize(const ws::rectangle_t *r)
        {
            // Call parent class to realize
            WidgetContainer::realize(r);
            lltl::parray<AudioChannel> channels;
            get_visible_items(&channels);

            // Compute the size of area
            float scaling   = lsp_max(0.0f, sScaling.get());
            float xr        = lsp_max(0.0f, ceilf(sBorderRadius.get() * scaling));  // external radius
            float bw        = lsp_max(0.0f, ceilf(sBorder.get() * scaling));        // border size
            float ir        = lsp_max(0.0f, xr - bw);                               // internal radius
            ssize_t padding = ceilf((1.0f - M_SQRT1_2) * ir + bw);                  // padding of internal area

            sGraph.nLeft    = r->nLeft   + padding;
            sGraph.nTop     = r->nTop    + padding;
            sGraph.nWidth   = r->nWidth  - padding*2;
            sGraph.nHeight  = r->nHeight - padding*2;

            sIPadding.enter(&sGraph, scaling);
        }

        void AudioSample::draw_channel1(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c, size_t samples, float scaling, float bright)
        {
            // TODO
        }

        void AudioSample::draw_fades1(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c, size_t samples, float scaling, float bright)
        {
            // TODO
        }

        void AudioSample::draw_channel2(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c1, AudioChannel *c2, size_t samples, float scaling, float bright)
        {
            // TODO
        }

        void AudioSample::draw_fades2(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c1, AudioChannel *c2, size_t samples, float scaling, float bright)
        {
            // TODO
        }

        void AudioSample::draw(ws::ISurface *s)
        {
//            float scaling       = lsp_max(0.0f, sScaling.get());
//            bool sgroups        = sSGroups.get();

            lsp::Color red(1.0f, 0.0f, 0.0f);
            s->clear(&red);
        }

        void AudioSample::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            float scaling   = lsp_max(0.0f, sScaling.get());
            float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius
            float bw        = lsp_max(0.0f, sBorder.get() * scaling);       // border size
            float bright    = sBrightness.get();
            bool pressed    = nXFlags & XF_DOWN;

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
                cv  = get_surface(s, sGraph.nWidth, sGraph.nHeight);
                if (cv != NULL)
                {
                    // Draw the surface
                    if (pressed)
                    {
                        ssize_t xbw         = lsp_max(1.0f, scaling);
                        ws::rectangle_t xr  = sGraph;
                        xr.nLeft           += xbw;
                        xr.nTop            += xbw;
                        xr.nWidth           = lsp_max(0, xr.nWidth  - xbw * 2);
                        xr.nHeight          = lsp_max(0, xr.nHeight - xbw * 2);

                        s->draw(cv, &xr);
                    }
                    else
                        s->draw(cv, sGraph.nLeft, sGraph.nTop);
                }

                // Draw the glass and the border
                color.copy(sGlassColor);
                bg_color.copy(sColor);
                color.scale_lightness(bright);
                bg_color.scale_lightness(bright);

                // Update border width if widget is in pressed state
                if (pressed)
                    bw         += lsp_max(1.0f, scaling);

                if (sGlass.get())
                {
                    cv = create_border_glass(&pGlass, s,
                            color, bg_color,
                            SURFMASK_ALL_CORNER, bw, xr,
                            sSize.nWidth, sSize.nHeight
                        );
                    if (cv != NULL)
                        s->draw(cv, sSize.nLeft, sSize.nTop);
                }
                else
                {
                    drop_glass();
                    draw_border(s, bg_color, SURFMASK_ALL_CORNER, bw, xr, &sSize);
                }

                s->set_antialiasing(aa);
            }
            s->clip_end();
        }

        void AudioSample::hide_widget()
        {
            WidgetContainer::hide_widget();
            drop_glass();
        }

        void AudioSample::get_visible_items(lltl::parray<AudioChannel> *dst)
        {
            for (size_t i=0, n=vChannels.size(); i<n; ++i)
            {
                AudioChannel *c = vChannels.get(i);
                if ((c == NULL) || (!c->visibility()->get()))
                    continue;
                if (!dst->add(c))
                    return;
            }
        }

        void AudioSample::on_add_item(void *obj, Property *prop, Widget *w)
        {
            AudioChannel *item = widget_cast<AudioChannel>(w);
            if (item == NULL)
                return;

            AudioSample *_this = widget_ptrcast<AudioSample>(obj);
            if (_this == NULL)
                return;

            item->set_parent(_this);
            _this->query_resize();
        }

        void AudioSample::on_remove_item(void *obj, Property *prop, Widget *w)
        {
            AudioChannel *item = widget_cast<AudioChannel>(w);
            if (item == NULL)
                return;

            AudioSample *_this = widget_ptrcast<AudioSample>(obj);
            if (_this == NULL)
                return;

            // Remove widget from supplementary structures
            _this->unlink_widget(item);
            _this->query_resize();
        }

        status_t AudioSample::add(Widget *widget)
        {
            AudioChannel *item  = widget_cast<AudioChannel>(widget);
            return (item != NULL) ? vChannels.add(item) : STATUS_BAD_TYPE;
        }

        status_t AudioSample::remove(Widget *child)
        {
            AudioChannel *item  = widget_cast<AudioChannel>(child);
            return (item != NULL) ? vChannels.premove(item) : STATUS_BAD_TYPE;
        }

        status_t AudioSample::remove_all()
        {
            vChannels.clear();
            return STATUS_OK;
        }

        status_t AudioSample::on_mouse_down(const ws::event_t *e)
        {
            // Handle mouse event
            float scaling   = lsp_max(0.0f, sScaling.get());
            float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius

            if (nBMask == 0)
            {
                nBMask |= 1 << e->nCode;
                if (Position::rinside(&sSize, e->nLeft, e->nTop, xr))
                {
                    if (e->nCode == ws::MCB_LEFT)
                        nXFlags    |= XF_LBUTTON;
                    else if (e->nCode == ws::MCB_RIGHT)
                        nXFlags    |= XF_RBUTTON;
                }
            }

            return handle_mouse_move(e);
        }

        status_t AudioSample::on_mouse_up(const ws::event_t *e)
        {
            size_t mask = nBMask;
            nBMask &= ~(1 << e->nCode);

            if (mask == (1U << e->nCode))
            {
                size_t flags    = nXFlags;
                nXFlags         = 0;

                float scaling   = lsp_max(0.0f, sScaling.get());
                float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius

                if (Position::rinside(&sSize, e->nLeft, e->nTop, xr))
                {
                    if ((e->nCode == ws::MCB_LEFT) && (flags & XF_LBUTTON))
                    {
                        if (sActive.get())
                            sSlots.execute(SLOT_SUBMIT, this, NULL);
                    }
                    else if ((e->nCode == ws::MCB_RIGHT) && (flags & XF_RBUTTON))
                    {
                        Menu *popup = sPopup.get();
                        if (popup != NULL)
                        {
                            ws::rectangle_t sr;
                            Window *wnd = widget_cast<Window>(this->toplevel());
                            wnd->get_screen_rectangle(&sr);
                            sr.nLeft       += e->nLeft;
                            sr.nTop        += e->nTop;
                            popup->show(this, sr.nLeft, sr.nTop);
                        }
                    }
                }

                if (flags != nXFlags)
                {
                    drop_glass();
                    query_draw();
                }
            }

            return STATUS_OK;
        }

        status_t AudioSample::on_mouse_move(const ws::event_t *e)
        {
            // Widget is not active?
            if (nXFlags == 0)
                return STATUS_OK;

            return handle_mouse_move(e);
        }

        status_t AudioSample::handle_mouse_move(const ws::event_t *e)
        {
            if (nXFlags & XF_LBUTTON)
            {
                float scaling   = lsp_max(0.0f, sScaling.get());
                float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius
                bool pressed    = (sActive.get()) && (nBMask & ws::MCF_LEFT) && (Position::rinside(&sSize, e->nLeft, e->nTop, xr));

                size_t old      = nXFlags;
                nXFlags         = lsp_setflag(nXFlags, XF_DOWN, pressed);

                if (old != nXFlags)
                {
                    drop_glass();
                    query_draw();
                }
            }

            return STATUS_OK;
        }
    }
}


