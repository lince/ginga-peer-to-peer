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
 * @file IP2PManager.h
 * @author Armando Biagioni Neto
 * @author Diogo de Carvalho Pedrosa
 * @author José Augusto Costa Martins Júnior
 * @date 22-07-10
 */

#ifndef IP2PMANAGER_H
#define IP2PMANAGER_H

#include <string>
#include <vector>
using namespace std;

#include "IP2PEventListener.h"

// TODO Retirar esses includes daqui para não expor nessa interface nada
// relacionado à libjingle.
#include <talk/base/cryptstring.h>				// Para talk_base::InsecureCryptStringImpl
#include <talk/examples/login/status.h>			// Para buzz::Status

namespace br {
namespace usp {
namespace icmc {
namespace intermidia {
namespace ginga {
namespace core {
namespace p2p {

/**
 * Interface do gerenciador de conexões P2P.
 */
class IP2PManager {
public:
	/**
	 * \enum
	 * Possíveis valores para definir o status do usuário.
	 */
	enum Status {
		NONE     = 0, /*!< Usado quando o status não estiver definido. */
		OFFLINE  = 1, /*!< Usado quando o usuário estiver desconectado. */
		XA       = 2, /*!< eXtended Away - Ausência estendida. */
		AWAY     = 3, /*!< Ausente */
		DND      = 4, /*!< Do Not Disturb - Não pertube. */
		ONLINE   = 5, /*!< Disponível. */
		CHAT     = 6, /*!< Querendo conversar. */
	};

	/**
	 * Destói o objeto P2PManager.
	 */
	virtual ~IP2PManager(){};

	/**
	 * Conecta ao servidor/porta desejados usando o nome de usuário e
	 * senha fornecidos.
	 *
	 * @param server Servidor a ser conectado.
	 * @param port Porta usada para conexão com servidor.
	 * @param username Nome do usuário utilizado na conexão.
	 * @param pass Senha utilizado na conexão.
	 * @param listener Observador de eventos relacionados à conexão P2P, como
	 * mudança do estado da conexão e de transferências de arquivos ou mudança
	 * de status de algum amigo.
	 */
	virtual void connect(string server, int port, string username,
			talk_base::InsecureCryptStringImpl pass,
			IP2PEventListener* listener = NULL) = 0;

	/**
	 * Disconecta do servidor.
	 */
	virtual void disconnect() = 0;

	/**
	 * Envia arquivos e diretórios para um determinado amigo.
	 *
	 * @param files Vetor contendo os caminhos dos arquivos e diretórios que
	 * serão enviados
	 * @param jid Identificador o amigo que irá receber os arquivos
	 */
	virtual void sendFiles(vector<string>* files, string username) = 0;

	/**
	 * Envia arquivo ou diretório para um determinado amigo.
	 *
	 * @param file Caminho do arquivo ou diretório que será enviado
	 * @param jid Identificador o amigo que irá receber os arquivos
	 */
	virtual void sendFile(string file, string username) = 0;

	/**
	 * Envia uma mensagem de texto para um determinado amigo.
	 *
	 * @param username Identificador o amigo que irá receber os arquivos
	 * @param text Mensagem a ser enviada
	 */
	virtual void sendChatMessage(string username, string text) = 0;

	/**
	 * Retorna a lista de amigos conectados.
	 *
	 * @return Um vetor com o status dos amigos conectados.
	 * completo.
	 */
	virtual vector<buzz::Status>* getFriends() = 0;

	/**
	 * Retorna a lista de amigos conectados.
	 *
	 * @return Um mapa que relaciona o identificador do amigo (jid completo em
	 * forma de string) com seu status completo.
	 */
	virtual map<string, buzz::Status>* getFriendsMap() = 0;

	/**
	 * Permite definir o próprio status. Pode ser chamado antes ou depois de se
	 * conectar.
	 *
	 * @param s Status a ser utilizado. @see Status.
	 * @param message Mensagem personalizada opcional.
	 */
	virtual void setStatus(Status s, string message) = 0;

	/**
	 * Permite acessar o próprio status. Pode ser chamado antes ou depois de se
	 * conectar.
	 *
	 * @return Status completo.
	 */
	virtual buzz::Status* getStatus() = 0;

	/**
	 * Aceita o recebimento de um arquivo.
	 */
	virtual void accept() = 0;

	/**
	 * Rejeita o recebimento de um arquivo.
	 */
	virtual void reject() = 0;

};
}
}
}
}
}
}
}

typedef ::br::usp::icmc::intermidia::ginga::core::p2p::IP2PManager*
P2PManagerCreator();

typedef void P2PManagerDestroyer(
		::br::usp::icmc::intermidia::ginga::core::p2p::IP2PManager* p2pm);

#endif /*IP2PMANAGER_H*/
