/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Copyright (C) 2009 UFSCar/Lince, Todos os Direitos Reservados.

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
mgp@icmc.usp.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.icmc.usp.br/php/laboratorio.php?origem=icmc&id_lab=3&nat=icmc
 ******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright (C) 2009 UFSCar/Lince, Todos os Direitos Reservados.

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
mgp@icmc.usp.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.icmc.usp.br/php/laboratorio.php?origem=icmc&id_lab=3&nat=icmc
 *******************************************************************************/

/**
 * @file IP2PEventListener.h
 * @author Diogo de Carvalho Pedrosa
 * @date 13-08-10
 */
#ifndef IP2PEVENTLISTENER_H
#define IP2PEVENTLISTENER_H

#include "FileShareClient.h"
#include <talk/xmpp/xmppengine.h>				// Para buzz::XmppEngine::State

namespace br {
namespace usp {
namespace icmc {
namespace intermidia {
namespace ginga {
namespace core {
namespace p2p {

/**
 * Interface que representa um observador de eventos P2P.
 */
class IP2PEventListener {
public:
	/**
	 * Destrói o IP2PEventListener.
	 *
	 */
	virtual ~IP2PEventListener(){};

	/**
	 * Notifica oferta para receber um arquivo ou pasta ou alguma mudança de
	 * estado relacionada a uma transferência de arquivo ou pasta.
	 *
	 * @param state Indica o que ocorreu.
	 * @param manifest Descrição dos itens que estão sendo oferecidos. Só é
	 * enviado quando o state é OFFER.
	 */
	virtual void transferStateChanged(FileShareClient::State state,
			const cricket::FileShareManifest* manifest) = 0;

	/**
	 * Notifica mudança de estado da conexão do P2PManager.
	 *
	 * @param state Indica o novo estado da conexão.
	 */
	virtual void stateChanged(buzz::XmppEngine::State state) = 0;

	/**
	 * Notifica mudança de status de algum amigo.
	 *
	 * @param status Indica novo status de um amigo.
	 */
	virtual void friendsStatusChanged(const buzz::Status &status) = 0;

	/**
	 * Notifica recebimento de mensagens de texto.
	 *
	 * @param from Bare jid do remetente em forma de string.
	 * @param text Mensagem enviada.
	 */
	virtual void chatMessageReceived(const string& from,
			const string& text) = 0;
};
}
}
}
}
}
}
}

#endif /* IP2PEVENTLISTENER_H */
