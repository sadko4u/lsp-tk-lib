/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 27 мая 2020 г.
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

namespace lsp
{
    namespace tk
    {
        const prop::desc_t Font::DESC[] =
        {
            { ".name",      PT_STRING   },
            { ".size",      PT_FLOAT    },
            { ".flags",     PT_STRING   },
            { ".bold",      PT_BOOL     },
            { ".italic",    PT_BOOL     },
            { ".underline", PT_BOOL     },
            { ".aliasing",  PT_BOOL     },
            { NULL,         PT_UNKNOWN  }
        };

        const prop::enum_t Font::FLAGS[] =
        {
            { "bold",       ws::FF_BOLD             },
            { "italic",     ws::FF_ITALIC           },
            { "underline",  ws::FF_UNDERLINE        },
            { "aliasing",   ws::FF_ANTIALIASING     },
            { NULL,         0                       }
        };

        Font::Font(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener)
        {
            nOverride       = 0;
        }

        Font::~Font()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Font::push_masked(size_t mask)
        {
            ws::Font &f = sValue;

            // Simple components
            if ((mask & O_NAME) && (vAtoms[P_NAME] >= 0))
                pStyle->set_string(vAtoms[P_NAME], f.get_name());
            if ((mask & O_SIZE) && (vAtoms[P_SIZE] >= 0))
                pStyle->set_float(vAtoms[P_SIZE], f.get_size());
            if ((mask & O_BOLD) && (vAtoms[P_BOLD] >= 0))
                pStyle->set_bool(vAtoms[P_BOLD], f.is_bold());
            if ((mask & O_ITALIC) && (vAtoms[P_ITALIC] >= 0))
                pStyle->set_bool(vAtoms[P_ITALIC], f.is_italic());
            if ((mask & O_UNDERLINE) && (vAtoms[P_UNDERLINE] >= 0))
                pStyle->set_bool(vAtoms[P_UNDERLINE], f.is_underline());
            if ((mask & O_ANTIALIAS) && (vAtoms[P_ANTIALIAS] >= 0))
                pStyle->set_bool(vAtoms[P_ANTIALIAS], f.is_antialiasing());

            // Complicated properties
            LSPString s;
            if ((mask & O_FLAGS) && (vAtoms[P_FLAGS] >= 0))
            {
                Property::fmt_bit_enums(&s, FLAGS, f.flags());
                pStyle->set_string(vAtoms[P_FLAGS], &s);
            }
        }

        void Font::push()
        {
            push_masked(O_ALL);
        }

        void Font::override()
        {
            if (pStyle == NULL)
                return;
            if (!pStyle->config_mode())
                return;

            // Push changes to style
            pStyle->begin(&sListener);
                bool over = pStyle->set_override(true);
                push_masked(nOverride);
                nOverride = 0;
                pStyle->set_override(over);
            pStyle->end();

            // Notify listeners about changes
            if (pListener != NULL)
                pListener->notify(this);
        }

        void Font::commit(atom_t property)
        {
            ws::Font &f = sValue;

            const char *sname;
            bool bvalue;
            float fvalue;
            size_t flags;
            LSPString s;

            if ((property == vAtoms[P_NAME]) && (pStyle->get_string(vAtoms[P_NAME], &sname) == STATUS_OK))
                f.set_name(sname);

            if ((property == vAtoms[P_SIZE]) && (pStyle->get_float(vAtoms[P_SIZE], &fvalue) == STATUS_OK))
                f.set_size(lsp_max(0.0f, fvalue));

            if ((property == vAtoms[P_BOLD]) && (pStyle->get_bool(vAtoms[P_BOLD], &bvalue) == STATUS_OK))
                f.set_bold(bvalue);
            if ((property == vAtoms[P_ITALIC]) && (pStyle->get_bool(vAtoms[P_ITALIC], &bvalue) == STATUS_OK))
                f.set_italic(bvalue);
            if ((property == vAtoms[P_UNDERLINE]) && (pStyle->get_bool(vAtoms[P_UNDERLINE], &bvalue) == STATUS_OK))
                f.set_underline(bvalue);
            if ((property == vAtoms[P_ANTIALIAS]) && (pStyle->get_bool(vAtoms[P_ANTIALIAS], &bvalue) == STATUS_OK))
                f.set_antialiasing(bvalue);

            if ((property == vAtoms[P_FLAGS]) && (pStyle->get_string(vAtoms[P_FLAGS], &s) == STATUS_OK))
            {
                if (Property::parse_bit_enums(&flags, &s, FLAGS) >= 0)
                    f.set_flags(flags);
            }
        }

        void Font::get(ws::Font *f, float scaling) const
        {
            f->set(sValue.get_name(), sValue.get_size() * lsp_max(0.0f, scaling), sValue.flags());
        }

        void Font::set_name(const char *name)
        {
            nOverride      |= O_NAME;
            const char *old = sValue.get_name();
            if (old == name)
                return;
            if ((old != NULL) && (name != NULL) && (::strcmp(old, name) == 0))
                return;

            sValue.set_name(name);
            sync();
        }

        float Font::set_size(float size)
        {
            nOverride      |= O_SIZE;
            float old = sValue.get_size();
            if (old == size)
                return old;

            sValue.set_size(size);
            sync();
            return old;
        }

        bool Font::set_bold(bool on)
        {
            nOverride      |= O_FLAGS | O_BOLD;
            bool old = sValue.is_bold();
            if (old == on)
                return old;

            sValue.set_bold(on);
            sync();
            return old;
        }

        bool Font::set_italic(bool on)
        {
            nOverride      |= O_FLAGS | O_ITALIC;
            bool old = sValue.is_italic();
            if (old == on)
                return old;

            sValue.set_italic(on);
            sync();
            return old;
        }

        bool Font::set_underline(bool on)
        {
            nOverride      |= O_FLAGS | O_UNDERLINE;
            bool old = sValue.is_underline();
            if (old == on)
                return old;

            sValue.set_underline(on);
            sync();
            return old;
        }

        bool Font::set_antialiasing(bool on)
        {
            nOverride      |= O_FLAGS | O_ANTIALIAS;
            bool old = sValue.is_antialiasing();
            if (old == on)
                return old;

            sValue.set_antialiasing(on);
            sync();
            return old;
        }

        size_t Font::set_flags(size_t flags)
        {
            nOverride      |= O_ALL_FLAGS;
            flags      &= ws::FF_ALL;
            size_t old  = sValue.flags();
            if (old == flags)
                return old;

            sValue.set_flags(flags);
            sync();
            return old;
        }

        void Font::set_params(size_t size, size_t flags)
        {
            nOverride      |= O_SIZE | O_ALL_FLAGS;
            flags              &= ws::FF_ALL;

            if ((flags == sValue.flags()) &&
                (size == sValue.get_size()))
                return;

            sValue.set_size(size);
            sValue.set_flags(flags);
            sync();
        }

        void Font::set(const char *name, size_t size, size_t flags)
        {
            nOverride      |= O_ALL;
            bool changed = false;

            // Change name
            const char *oname = sValue.get_name();
            if (oname == name) {} // Nothing
            else if ((oname != NULL) && (name != NULL) && (::strcmp(oname, name) == 0)) {} // Nothing
            else
            {
                sValue.set_name(name);
                changed     = true;
            }

            // Change size
            float osize = sValue.get_size();
            if (size != osize)
            {
                sValue.set_size(size);
                changed     = true;
            }

            // Change flags
            size_t oflags = sValue.flags();
            if (flags != oflags)
            {
                sValue.set_flags(flags);
                changed     = true;
            }

            // Sync
            if (changed)
                sync();
        }

        void Font::set(const ws::Font *f)
        {
            set(f->get_name(), f->get_size(), f->flags());
        }

        bool Font::get_parameters(ws::ISurface *s, float scaling, ws::font_parameters_t *fp) const
        {
            if (s == NULL)
                return false;
            ws::Font f(sValue.get_name(), sValue.get_size() * lsp_max(0.0f, scaling), sValue.flags());
            return s->get_font_parameters(f, fp);
        }

        bool Font::get_parameters(Display *dpy, float scaling, ws::font_parameters_t *fp) const
        {
            ws::IDisplay *xdpy = (dpy != NULL) ? dpy->display() : NULL;
            ws::ISurface *s = (xdpy != NULL) ? xdpy->estimation_surface() : NULL;
            return (s != NULL) ? get_parameters(s, scaling, fp) : false;
        }

        bool Font::get_multitext_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const LSPString *text) const
        {
            if (text == NULL)
                return false;
            ws::IDisplay *xdpy = (dpy != NULL) ? dpy->display() : NULL;
            ws::ISurface *s = (xdpy != NULL) ? xdpy->estimation_surface() : NULL;
            return (s != NULL) ? get_multitext_parameters(s, tp, scaling, text, 0, text->length()) : false;
        }

        bool Font::get_multitext_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first) const
        {
            if (text == NULL)
                return false;
            ws::IDisplay *xdpy = (dpy != NULL) ? dpy->display() : NULL;
            ws::ISurface *s = (xdpy != NULL) ? xdpy->estimation_surface() : NULL;
            return (s != NULL) ? get_multitext_parameters(s, tp, scaling, text, first, text->length()) : false;
        }

        bool Font::get_multitext_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first, ssize_t last) const
        {
            if (text == NULL)
                return false;
            ws::IDisplay *xdpy = (dpy != NULL) ? dpy->display() : NULL;
            ws::ISurface *s = (xdpy != NULL) ? xdpy->estimation_surface() : NULL;
            return (s != NULL) ? get_multitext_parameters(s, tp, scaling, text, first, last) : false;
        }

        bool Font::get_multitext_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const LSPString *text) const
        {
            return (text != NULL) ? get_multitext_parameters(s, tp, scaling, text, 0, text->length()) : false;
        }

        bool Font::get_multitext_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first) const
        {
            return (text != NULL) ? get_multitext_parameters(s, tp, scaling, text, first, text->length()) : false;
        }

        bool Font::get_multitext_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first, ssize_t last) const
        {
            if ((s == NULL) || (text == NULL))
                return false;

            ws::Font f(sValue.get_name(), sValue.get_size() * lsp_max(0.0f, scaling), sValue.flags());

            ssize_t prev = 0, curr = 0, tail = 0;
            ws::font_parameters_t fp;
            ws::text_parameters_t xp;

            if (!s->get_font_parameters(f, &fp))
                return false;

            float w = 0, h = 0;

            while (curr < last)
            {
                curr    = text->index_of(prev, '\n');
                if ((curr < 0) || (curr > last))
                {
                    curr        = last;
                    tail        = last;
                }
                else
                {
                    tail        = curr;
                    if ((tail > last) && (text->at(tail-1) == '\r'))
                        --tail;
                }

                // Get text parameters
                const char *str = text->get_utf8(prev, tail);
                if (str == NULL)
                    return false;

                if (!s->get_text_parameters(f, &xp, str))
                    return false;

                if (w < xp.Width)
                    w       = xp.Width;
                h      += fp.Height;

                prev    = curr + 1;
            }

            // Store font parameters
            xp.Width    = w;
            xp.Height   = h;
            *tp         = xp;
            return true;
        }

        bool Font::get_text_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const LSPString *text) const
        {
            if (text == NULL)
                return false;
            ws::IDisplay *xdpy = (dpy != NULL) ? dpy->display() : NULL;
            ws::ISurface *s = (xdpy != NULL) ? xdpy->estimation_surface() : NULL;
            return (s != NULL) ? get_text_parameters(s, tp, scaling, text, 0, text->length()) : false;
        }

        bool Font::get_text_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first) const
        {
            if (text == NULL)
                return false;
            ws::IDisplay *xdpy = (dpy != NULL) ? dpy->display() : NULL;
            ws::ISurface *s = (xdpy != NULL) ? xdpy->estimation_surface() : NULL;
            return (s != NULL) ? get_text_parameters(s, tp, scaling, text, first, text->length()) : false;
        }

        bool Font::get_text_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first, ssize_t last) const
        {
            if (text == NULL)
                return false;
            ws::IDisplay *xdpy = (dpy != NULL) ? dpy->display() : NULL;
            ws::ISurface *s = (xdpy != NULL) ? xdpy->estimation_surface() : NULL;
            return (s != NULL) ? get_text_parameters(s, tp, scaling, text, first, last) : false;
        }

        bool Font::get_text_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const LSPString *text) const
        {
            return (text != NULL) ? get_text_parameters(s, tp, scaling, text, 0, text->length()) : false;
        }

        bool Font::get_text_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first) const
        {
            return (text != NULL) ? get_text_parameters(s, tp, scaling, text, 0, text->length()) : false;
        }

        bool Font::get_text_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first, ssize_t last) const
        {
            if (s == NULL)
                return false;

            ws::Font f(sValue.get_name(), sValue.get_size() * lsp_max(0.0f, scaling), sValue.flags());
            return s->get_text_parameters(f, tp, text, first, last);
        }

        bool Font::get_text_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const char *text) const
        {
            LSPString tmp;
            return (tmp.set_utf8(text)) ? get_text_parameters(s, tp, scaling, &tmp, 0, tmp.length()) : false;
        }

        bool Font::get_text_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const char *text, ssize_t first) const
        {
            LSPString tmp;
            return (tmp.set_utf8(text)) ? get_text_parameters(s, tp, scaling, &tmp, first, tmp.length()) : false;
        }

        bool Font::get_text_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const char *text, ssize_t first, ssize_t last) const
        {
            LSPString tmp;
            return (tmp.set_utf8(text)) ? get_text_parameters(s, tp, scaling, &tmp, first, last) : false;
        }

        bool Font::get_text_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const char *text) const
        {
            LSPString tmp;
            return (tmp.set_utf8(text)) ? get_text_parameters(dpy, tp, scaling, &tmp, 0, tmp.length()) : false;
        }

        bool Font::get_text_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const char *text, ssize_t first) const
        {
            LSPString tmp;
            return (tmp.set_utf8(text)) ? get_text_parameters(dpy, tp, scaling, &tmp, first, tmp.length()) : false;
        }

        bool Font::get_text_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const char *text, ssize_t first, ssize_t last) const
        {
            LSPString tmp;
            return (tmp.set_utf8(text)) ? get_text_parameters(dpy, tp, scaling, &tmp, first, last) : false;
        }

        void Font::draw(ws::ISurface *s, const lsp::Color &c, float x, float y, float scaling, const LSPString *text) const
        {
            if (text != NULL)
                draw(s, c, x, y, scaling, text, 0, text->length());
        }

        void Font::draw(ws::ISurface *s, const lsp::Color &c, float x, float y, float scaling, const LSPString *text, size_t first) const
        {
            if (text != NULL)
                draw(s, c, x, y, scaling, text, first, text->length());
        }

        void Font::draw(ws::ISurface *s, const lsp::Color &c, float x, float y, float scaling, const LSPString *text, size_t first, size_t last) const
        {
            if (s == NULL)
                return;

            ws::Font f(sValue.get_name(), sValue.get_size() * lsp_max(0.0f, scaling), sValue.flags());
            s->out_text(f, c, x, y, text, first, last);
        }
    }
}


