/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 21 авг. 2019 г.
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
#include <lsp-plug.in/io/InMemoryStream.h>

namespace lsp
{
    namespace tk
    {
        static const char * const mimes[] =
        {
            "UTF8_STRING",
            "text/plain;charset=utf-8",
            "text/plain;charset=UTF-16LE",
            "text/plain;charset=UTF-16BE",
            "text/plain;charset=US-ASCII",
            "text/plain",
            NULL
        };
        
        TextDataSource::TextDataSource(): IDataSource(mimes)
        {
        }
        
        TextDataSource::~TextDataSource()
        {
            sText.truncate();
        }

        io::IInStream *TextDataSource::open(const char *mime)
        {
            // Scan supported MIME types
            ssize_t idx = -1, i=0;
            for (const char *const *p = mimes; *p != NULL; ++p, ++i)
            {
                if (!::strcasecmp(mimes[i], mime))
                {
                    idx = i;
                    break;
                }
            }

            // Analyze found MIME type
            void *data      = NULL;
            size_t bytes    = 0;
            switch (idx)
            {
                case 0: // UTF8_STRING
                case 1: // text/plain;charset=utf-8
                    data    = sText.clone_utf8(&bytes);
                    bytes  -= sizeof(char);             // 1 extra byte for zero character
                    break;
                case 2: // text/plain;charset=UTF-16LE
                    data    = __IF_LEBE(
                            sText.clone_utf16(&bytes),
                            sText.clone_native(&bytes, "UTF16-LE")
                        );
                    bytes  -= sizeof(lsp_utf16_t);      // 2 extra bytes for zero character
                    break;
                case 3: // text/plain;charset=UTF-16BE
                    data = __IF_LEBE(
                            sText.clone_native(&bytes, "UTF16-BE"),
                            sText.clone_utf16(&bytes)
                        );
                    bytes  -= sizeof(lsp_utf16_t);      // 2 extra bytes for zero character
                    break;
                case 4:
                    data    = sText.clone_ascii(&bytes);
                    bytes  -= sizeof(char);             // 1 extra byte for zero character
                    break;
                case 5:
                    data    = sText.clone_native(&bytes);
                    bytes  -= sizeof(char);             // 4 extra byte for zero character
                    break;
                default:
                    break;
            }

            // Format not supported?
            if (data == NULL)
                return NULL;

            // Allocate memory stream;
            io::InMemoryStream *stream = new io::InMemoryStream(data, bytes, MEMDROP_FREE);
            if (stream == NULL)
            {
                ::free(data);
                return NULL;
            }

            return stream;
        }

        status_t TextDataSource::set_text(const char *text)
        {
            return sText.set_utf8(text) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t TextDataSource::set_text(const LSPString *text)
        {
            return sText.set(text) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t TextDataSource::set_text(const LSPString *text, ssize_t first)
        {
            return sText.set(text, first) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t TextDataSource::set_text(const LSPString *text, ssize_t first, ssize_t last)
        {
            return sText.set(text, first, last) ? STATUS_OK : STATUS_NO_MEM;
        }
    
    } /* namespace tk */
} /* namespace lsp */
