/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 USP/Laboratorio Intermidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
http://www.ncl.org.br
http://www.ginga.org.br
http://agua.intermidia.icmc.usp.br/intermidia
 ******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 USP/LABORATORIO INTERMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
Maria da Graça Pimentel - mgp@icmc.usp.br
http://www.ncl.org.br
http://www.ginga.org.br
http://agua.intermidia.icmc.usp.br/intermidia
 *******************************************************************************/

/**
 * @file ChatTask.h
 * @author Diogo de Carvalho Pedrosa
 * @author José Augusto Costa Martins Júnior
 * @date 10-08-10
 */

#include "../include/ChatTask.h"

#include <talk/xmpp/constants.h>

//#include <iostream>
//#include <time.h>

namespace br {
namespace usp {
namespace icmc {
namespace intermidia {
namespace ginga {
namespace core {
namespace p2p {

// HL_TYPE significa que esta classe está interessada em ser notificada apenas
// da ocorrência de um tipo específico de mensagem, como por exemplo mensagens
// do tipo groupchat.
ChatTask::ChatTask(talk_base::Task* parent) :
		XmppTask(parent, buzz::XmppEngine::HL_TYPE) {
	logger = LoggerUtil::getLogger("ginga.core.p2p.ChatTask");
	LoggerUtil::configure();
	LoggerUtil_info(logger, "Construtor");
}

ChatTask::~ChatTask() {
	LoggerUtil_info(logger, "Destrutor");
}

// O handler deve retornar true se tratou a stanza e false caso a stanza deve
// ser passada para o próximo handler registrado.
bool ChatTask::HandleStanza(const buzz::XmlElement* stanza) {
	LoggerUtil_info(logger, "HandleStanza");

	if (stanza->Attr(buzz::QN_TYPE) != "chat") {
		LoggerUtil_info(logger, "HandleStanza - stanza->Attr(buzz::QN_TYPE) != \"chat\"");
		return false;
	}

	if (!stanza->HasAttr(buzz::QN_FROM)) {
		LoggerUtil_info(logger, "HandleStanza - !stanza->HasAttr(buzz::QN_FROM)");
		return false;
	}

	if (!stanza->FirstNamed(buzz::QN_BODY)) {
		LoggerUtil_info(logger, "HandleStanza - !stanza->FirstNamed(buzz::QN_BODY)");
		return false;
	}

	buzz::Jid jid = buzz::Jid(stanza->Attr(buzz::QN_FROM));
	SignalIncomingChat(jid, stanza->FirstNamed(buzz::QN_BODY)->BodyText());

	LoggerUtil_info(logger, "Fim de HandleStanza");

	return true;
}

void ChatTask::sendChatMessage(buzz::Jid* sendToJid, const string& text) {
	LoggerUtil_info(logger, "sendChatMessage");

	buzz::XmlElement* message = new buzz::XmlElement(buzz::QN_MESSAGE);
	message->AddAttr(buzz::QN_TO, sendToJid->BareJid().Str());
	message->AddAttr(buzz::QN_TYPE, "chat");

	buzz::XmlElement* body = message->FindOrAddNamedChild(buzz::QN_BODY);
	body->SetBodyText(text);

	SendStanza(message);
	delete message;
}

int ChatTask::ProcessStart() {
	LoggerUtil_info(logger, "ProcessStart");

	return STATE_RESPONSE;
}

}
}
}
}
}
}
}

