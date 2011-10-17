/*
 * Copyright (c) 2005 INdT.
 * @author Andre Moreira Magalhaes <andre.magalh...@indt.org.br> 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "textsessionclient.h"

#include <talk/xmpp/constants.h>
#include <talk/xmpp/xmppclient.h>

#include <iostream>

namespace {
    
const std::string STR_CHAT("chat");

}

using namespace buzz;

namespace cricket {

bool TextSessionClient::HandleStanza(const XmlElement *stanza) 
{
    if (stanza->Name() != QN_MESSAGE)
        return false;
    if (stanza->HasAttr(QN_TYPE) && stanza->Attr(QN_TYPE) != STR_CHAT)
        return false;
    QueueStanza(stanza);
    return true;
}

int TextSessionClient::ProcessStart() 
{
    const XmlElement *stanza = NextStanza();
    if (stanza == NULL)
        return STATE_BLOCKED;

    const XmlChild *child = stanza->FirstChild();
    if (!child)
        return STATE_START;
    
    const XmlElement *body = child->AsElement();
    if (body != NULL) {
        std::cout << "received message " << body->BodyText().c_str() << 
            " from " << stanza->Attr(QN_FROM).c_str() << std::endl;
        XmlElement *result = new XmlElement(QN_MESSAGE);
        result->AddAttr(QN_FROM, GetClient()->jid().Str());
        result->AddAttr(QN_TO, stanza->Attr(QN_FROM));
        result->AddAttr(QN_TYPE, STR_CHAT);
        // result->AddAttr(QN_ID, stanza->Attr(QN_ID));
        result->AddElement(new XmlElement(QN_BODY));
        result->AddText("test reply", 1);
        SendStanza(result);
    }
    return STATE_START;
 }
}
