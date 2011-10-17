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


#ifndef CHATTASK_H
#define CHATTASK_H

#include <talk/xmpp/xmppengine.h>
#include <talk/xmpp/xmpptask.h>

#include <linceutil/LoggerUtil.h>
using namespace ::br::ufscar::lince::util;

namespace br {
namespace usp {
namespace icmc {
namespace intermidia {
namespace ginga {
namespace core {
namespace p2p {

/**
 * Esta classe foi inspirada na classe de mesmo nome do projeto gtalX
 * (http://sites.google.com/site/jozsefbekes/Home/gtalx)
 *
 * Tem como finalidade permitir o envio e recebimento de mensagens de texto.
 */
class ChatTask : public buzz::XmppTask {
protected:
	/// Componente auxiliar para realização de logs.
	HLoggerPtr logger;

	virtual int ProcessStart();
	virtual bool HandleStanza(const buzz::XmlElement* stanza);

public:
	/**
	 * Construtor da Task responsável pelo chat.
	 *
	 * @param parent Task pai.
	 */
	ChatTask(talk_base::Task* parent);

	/**
	 * Destrutor da Task responsável pelo chat.
	 */
	virtual ~ChatTask();

	/**
	 * Slot pelo qual serão notificadas ocorrências de recebimento mensagens
	 * de texto.
	 */
	sigslot::signal2<buzz::Jid /*from*/,
				   const string& /*text*/> SignalIncomingChat;

	/**
	 * Envia uma mensagem de texto para um determinado amigo.
	 *
	 * @param sendToJid Amigo que irá receber a mensagem
	 * @param text Mensagem a ser enviada
	 */
	void sendChatMessage(buzz::Jid* sendToJid, const string& text);
};

}
}
}
}
}
}
}

#endif // CHATTASK_H
