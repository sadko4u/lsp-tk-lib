/*
 * GraphDot.h
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHDOT_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHDOT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class GraphDot: public GraphItem
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum flags_t
                {
                    F_EDITABLE      = 1 << 0,
                    F_HIGHLIGHT     = 1 << 1,
                    F_EDITING       = 1 << 2,
                    F_FINE_TUNE     = 1 << 3
                };

                typedef struct param_t
                {
                    GraphDot                   *pDot;
                    prop::Boolean               sEditable;      // Editable flag
                    prop::RangeFloat            sValue;         // Value
                    prop::StepFloat             sStep;          // Step

                    explicit                param_t(GraphDot *dot, prop::Listener *lst);
                    void                    property_changed(Property *prop);
                    void                    init(const char *prefix);
                } param_t;

            private:
                GraphDot & operator = (const GraphDot &);

            protected:
                param_t                     sHValue;            // Horizontal value
                param_t                     sVValue;            // Vertical value
                param_t                     sZValue;            // The value associated with scrolling

                prop::Integer               sOrigin;            // Index of origin widget
                prop::Integer               sHAxis;             // Horizontal axis
                prop::Integer               sVAxis;             // Vertical axis
                prop::Integer               sSize;              // Size of the dot
                prop::Integer               sHoverSize;         // Size of the dot (when hover)
                prop::Integer               sBorderSize;        // Border size
                prop::Integer               sHoverBorderSize;   // Border size (when hover)
                prop::Integer               sGap;               // Gap size
                prop::Integer               sHoverGap;          // Gap size (when hover)
                prop::Color                 sColor;             // Color of the dot
                prop::Color                 sHoverColor;        // Color of the dot (when hover)
                prop::Color                 sBorderColor;       // Color of the border
                prop::Color                 sHoverBorderColor;  // Color of the border (when hover)

                size_t                      nXFlags;            // Extra flags
                size_t                      nMBState;           // Mouse button state
                ssize_t                     nMouseX;            // Mouse initial X position
                ssize_t                     nMouseY;            // Mouse initial Y position

            protected:
                void                        apply_motion(ssize_t x, ssize_t y, size_t flags);

                static status_t             slot_on_change(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                property_changed(Property *prop);

            public:
                explicit GraphDot(Display *dpy);
                virtual ~GraphDot();

                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(RangeFloat,         hvalue,             &sHValue.sValue)
                LSP_TK_PROPERTY(Boolean,            heditable,          &sHValue.sEditable)
                LSP_TK_PROPERTY(StepFloat,          hstep,              &sHValue.sStep)
                LSP_TK_PROPERTY(RangeFloat,         vvalue,             &sVValue.sValue)
                LSP_TK_PROPERTY(Boolean,            veditable,          &sVValue.sEditable)
                LSP_TK_PROPERTY(StepFloat,          vstep,              &sVValue.sStep)
                LSP_TK_PROPERTY(RangeFloat,         zvalue,             &sZValue.sValue)
                LSP_TK_PROPERTY(Boolean,            zeditable,          &sZValue.sEditable)
                LSP_TK_PROPERTY(StepFloat,          zstep,              &sZValue.sStep)

                LSP_TK_PROPERTY(Integer,            origin,             &sOrigin)
                LSP_TK_PROPERTY(Integer,            haxis,              &sHAxis)
                LSP_TK_PROPERTY(Integer,            vaxis,              &sVAxis)
                LSP_TK_PROPERTY(Integer,            size,               &sSize)
                LSP_TK_PROPERTY(Integer,            hover_size,         &sHoverSize)
                LSP_TK_PROPERTY(Integer,            border_size,        &sBorderSize)
                LSP_TK_PROPERTY(Integer,            hover_border_size,  &sHoverBorderSize)
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Color,              hover_color,        &sHoverColor)
                LSP_TK_PROPERTY(Color,              border_color,       &sBorderColor)
                LSP_TK_PROPERTY(Color,              hover_border_color, &sHoverBorderColor)

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                virtual bool                inside(ssize_t x, ssize_t y);

                virtual status_t            on_mouse_in(const ws::event_t *e);

                virtual status_t            on_mouse_out(const ws::event_t *e);

                virtual status_t            on_mouse_down(const ws::event_t *e);

                virtual status_t            on_mouse_up(const ws::event_t *e);

                virtual status_t            on_mouse_move(const ws::event_t *e);

                virtual status_t            on_change();
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHDOT_H_ */
