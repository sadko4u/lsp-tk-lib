/*
 * MultiProperty.h
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_MULTIPROPERTY_H_
#define LSP_PLUG_IN_TK_PROP_MULTIPROPERTY_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>

#include <lsp-plug.in/tk/style.h>
#include <lsp-plug.in/tk/prop/types.h>
#include <lsp-plug.in/tk/prop/Property.h>

namespace lsp
{
    namespace tk
    {
        class Style;
        class Display;

        /**
         * Floating-point property interface
         */
        class MultiProperty: public Property
        {
            private:
                Property & operator = (const Property *);

            protected:
                status_t        unbind(atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener);
                status_t        bind(
                        atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener,
                        const char *property, Style *style, Display *dpy
                );

                void            set_default(atom_t *atoms, const prop::desc_t *desc);

            protected:
                inline MultiProperty(prop::Listener *listener = NULL): Property(listener) {};
        };

    }
}

#endif /* LSP_PLUG_IN_TK_PROP_MULTIPROPERTY_H_ */
