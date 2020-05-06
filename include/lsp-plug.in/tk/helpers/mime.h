/*
 * mime.h
 *
 *  Created on: 27 авг. 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_HELPERS_MIME_H_
#define UI_TK_HELPERS_MIME_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/io/IInStream.h>
#include <lsp-plug.in/io/IInSequence.h>

namespace lsp
{
    namespace tk
    {
        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, const LSPString *src);
        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset = NULL);
        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, io::IInSequence *is);
        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset = NULL);

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, const LSPString *src);
        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset = NULL);
        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, io::IInSequence *is);
        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset = NULL);

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, const LSPString *src);
        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset = NULL);
        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, io::IInSequence *is);
        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset = NULL);
    }
}



#endif /* UI_TK_HELPERS_MIME_H_ */
