/*
 * Boolean.cpp
 *
 *  Created on: 7 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        void Boolean::Listener::notify(atom_t property)
        {
            if (pValue->nAtom == property)
                pValue->commit();
        }

        Boolean::Boolean(prop::Listener *listener):
            Property(listener),
            sListener(this)
        {
            nAtom       = -1;
            bValue      = 0.0f;
        }

        Boolean::~Boolean()
        {
            unbind();
        }

        status_t Boolean::unbind()
        {
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                status_t res = pStyle->unbind(nAtom, &sListener);
                if (res != STATUS_OK)
                    return res;
            }

            pStyle      = NULL;
            nAtom       = -1;

            return STATUS_NOT_BOUND;
        }

        status_t Boolean::bind(atom_t property, Style *style)
        {
            if ((style == NULL) || (property < 0))
                return STATUS_BAD_ARGUMENTS;

            // Unbind first
            status_t res;
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                res = pStyle->unbind(nAtom, &sListener);
                if (res != STATUS_OK)
                    return res;
            }

            // Bind to new handler
            style->begin();
            {
                res = style->bind(property, PT_BOOL, &sListener);
                if (res == STATUS_OK)
                {
                    pStyle      = style;
                    nAtom       = property;
                }
            }
            style->end();

            return res;
        }

        void Boolean::commit()
        {
            // Handle change: remember new value
            if (pStyle != NULL)
            {
                if (pStyle->get_bool(nAtom, &bValue) != STATUS_OK)
                    return;
            }

            // Delegate event
            if (pListener != NULL)
                pListener->notify(this);
        }

        bool Boolean::set(bool v)
        {
            bool prev = bValue;
            if (v == bValue)
                return prev;

            bValue  = v;
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                pStyle->begin(&sListener);
                pStyle->set_bool(nAtom, v);
                pStyle->end();
            }
            else if (pListener != NULL)
                pListener->notify(this);
            return prev;
        }

        void Boolean::swap(Boolean *dst)
        {
            if (dst == this)
                return;

            bool tmp = dst->bValue;
            dst->set(bValue);
            set(tmp);
        }

        namespace prop
        {
            status_t Boolean::bind(const char *property, Widget *widget)
            {
                if ((widget == NULL) || (property == NULL))
                    return STATUS_BAD_ARGUMENTS;

                Display *dpy    = widget->display();
                atom_t id       = (dpy != NULL) ? dpy->atom_id(property) : -1;
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Boolean::bind(id, widget->style());
            }

            status_t Boolean::bind(atom_t property, Widget *widget)
            {
                return tk::Boolean::bind(property, widget->style());
            }

            status_t Boolean::bind(const char *property, Atoms *atoms, Style *style)
            {
                if ((atoms == NULL) || (style == NULL) || (property < 0))
                    return STATUS_BAD_ARGUMENTS;

                atom_t id       = atoms->atom_id(property);
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Boolean::bind(id, style);
            }

            status_t Boolean::bind(atom_t property, Style *style)
            {
                return tk::Boolean::bind(property, style);
            }


        }

    } /* namespace calc */
} /* namespace lsp */



