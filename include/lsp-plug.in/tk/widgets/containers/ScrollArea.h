/*
 * ScrollArea.h
 *
 *  Created on: 17 июл. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_SCROLLAREA_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_SCROLLAREA_H_

namespace lsp
{
    namespace tk
    {
        /**
         * Scrolling area widget, provides container that may scroll it's contents if it does not fit into
         * the window
         */
        class ScrollArea: public WidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                Widget                 *pWidget;
                ScrollBar               sHBar;
                ScrollBar               sVBar;

                prop::Layout            sLayout;
                prop::SizeConstraints   sSizeConstraints;
                prop::Scrolling         sHScroll;
                prop::Scrolling         sVScroll;

            protected:
                void                    do_destroy();

            protected:
                virtual Widget         *find_widget(ssize_t x, ssize_t y);
                virtual void            property_changed(Property *prop);
                virtual void            size_request(ws::size_limit_t *r);
                virtual void            realize(const ws::rectangle_t *r);

            public:
                explicit                ScrollArea(Display *dpy);
                virtual                 ~ScrollArea();

                virtual status_t        init();
                virtual void            destroy();

            public:
                LSP_TK_PROPERTY(Layout,             layout,             &sLayout)
                LSP_TK_PROPERTY(SizeConstraints,    size_constraints,   &sSizeConstraints)
                LSP_TK_PROPERTY(Scrolling,          hscroll,            &sHScroll)
                LSP_TK_PROPERTY(Scrolling,          vscroll,            &sVScroll)

                LSP_TK_PROPERTY(ScrollBar,          hbar,               &sHBar)
                LSP_TK_PROPERTY(ScrollBar,          vbar,               &sVBar)

            public:
                virtual void            render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                virtual status_t        add(Widget *widget);

                virtual status_t        remove(Widget *widget);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_SCROLLAREA_H_ */
