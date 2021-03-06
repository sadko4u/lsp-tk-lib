/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 4 сент. 2017 г.
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

#ifndef LSP_PLUG_IN_TK_UTIL_TEXTCURSOR_H_
#define LSP_PLUG_IN_TK_UTIL_TEXTCURSOR_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Helper to handle text cursor position and events and properly draw text cursor
         */
        class TextCursor
        {
            protected:
                enum flags_t
                {
                    F_VISIBLE       = 1 << 0,
                    F_REPLACE       = 1 << 1,
                    F_SHINE         = 1 << 2
                };

            protected:
                ssize_t     nLocation;
                size_t      nFlags;
                size_t      nBlinkPeriod;
                Timer       sTimer;

            protected:
                static status_t     update_blink(ws::timestamp_t sched, ws::timestamp_t ts, void *arg);

                virtual ssize_t     limit(ssize_t value);
                virtual void        on_change();
                virtual void        on_blink();

                void                set_shining(bool blink = true);

            public:
                explicit            TextCursor(Display *dpy);
                virtual             ~TextCursor();

            public:
                inline bool         visible() const     { return nFlags & F_VISIBLE; };
                inline bool         inserting() const   { return !(nFlags & F_REPLACE); };
                inline bool         replacing() const   { return nFlags & F_REPLACE; };
                inline bool         shining() const     { return nFlags & F_SHINE; };
                inline size_t       blink_period() const  { return nBlinkPeriod; }
                inline ssize_t      value() const    { return nLocation; };
                inline ssize_t      location() const { return nLocation; };
                inline ssize_t      position() const { return nLocation; };

            public:
                void                hide();
                void                show();
                void                blink(bool blink = true);
                void                shine(bool shine = true) { blink(shine); };
                void                set_visibility(bool visible = true);
                void                set_inserting(bool value = true);
                void                set_replacing(bool value = true);
                void                toggle_mode();
                void                toggle_visibility();
                void                set_blink_period(size_t value);
                ssize_t             set(ssize_t location);
                ssize_t             move(ssize_t distance);

                inline ssize_t      set_value(ssize_t location)      { return set(location); };
                inline ssize_t      set_location(ssize_t location)   { return set(location); };
                inline ssize_t      set_position(ssize_t location)   { return set(location); };
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_UTIL_TEXTCURSOR_H_ */
