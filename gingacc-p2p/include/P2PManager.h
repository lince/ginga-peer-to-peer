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
 * @file P2PManager.h
 * @author Armando Biagioni Neto
 * @author Diogo de Carvalho Pedrosa
 * @author José Augusto Costa Martins Júnior
 * @date 22-07-10
 */

#ifndef P2PMANAGER_H
#define P2PMANAGER_H

#include "IP2PManager.h"
#include "ChatTask.h"

#include <talk/examples/login/xmpppump.h>	// Para XmppPump
#include <talk/examples/login/xmppsocket.h> // Para XmppSocket
#include <talk/examples/login/status.h>			// Para buzz::Status
#include <talk/xmpp/xmppclient.h>				// Para buzz::XmppClient
#include <talk/p2p/client/httpportallocator.h>	// Para cricket::HttpPortAllocator


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
 * Classe responsável por gerenciar conexões P2P.
 */
class P2PManager : public IP2PManager, public sigslot::has_slots<> {
private:
	/// Gerencia o processo de log in e cria a instância do XmppClient.
	XmppPump *pump;

	talk_base::Thread* main_thread;
	FileShareClient* fs_client;
	ChatTask* chatTask;
	buzz::XmppClientSettings* xcs;
	XmppSocket* xmppSocket;
	char currentWorkingDirectory[256];

	buzz::Status* my_status;
	map<string, buzz::Status>* roster_;
	vector<buzz::Status>* rosterVector_;
	talk_base::NetworkManager network_manager_;
	talk_base::scoped_ptr<cricket::HttpPortAllocator> port_allocator_;
	talk_base::scoped_ptr<cricket::SessionManager> session_manager_;

	/// Indica se está conectado ou se pedido de conexão foi feito.
	bool connected;

	/// Observador registrado para receber eventos relacionados à conexão P2P,
	/// como mudança do estado da conexão e de transferências de arquivos ou
	/// mudança de status de algum amigo.
	IP2PEventListener* listener;

	/// Componente auxiliar para realização de logs.
	HLoggerPtr logger;

	void onSignon();

	/**
	 * Método que será chamando cada vez que ocorrer alguma mudança de estado
	 * do usuário que está logando.
	 */
	void OnStateChange(buzz::XmppEngine::State state);

	/**
	 * Método chamado toda vez que algum amigo muda de status.
	 */
	void OnStatusUpdate(const buzz::Status &status);

	void OnJingleInfo(const std::string & relay_token,
			const std::vector<std::string> &relay_addresses,
			const std::vector<talk_base::SocketAddress> &stun_addresses);

	void OnChatMessageReceived(buzz::Jid from, const string& text);

public:
	/**
	 * Constrói um objeto P2PManager.
	 *
	 */
	P2PManager();

	/**
	 * Destói o objeto P2PManager.
	 */
	virtual ~P2PManager();

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
	void connect(string server, int port, string username,
			talk_base::InsecureCryptStringImpl pass,
			IP2PEventListener* listener = NULL);

	/**
	 * Disconecta do servidor.
	 */
	void disconnect();

	/**
	 * Envia arquivos e diretórios para um determinado amigo.
	 *
	 * @param files Vetor contendo os caminhos dos arquivos e diretórios que
	 * serão enviados
	 * @param username Identificador o amigo que irá receber os arquivos
	 */
	void sendFiles(vector<string>* files, string username);

	/**
	 * Envia arquivo ou diretório para um determinado amigo.
	 *
	 * @param file Caminho do arquivo ou diretório que será enviado
	 * @param jid Identificador o amigo que irá receber os arquivos
	 */
	void sendFile(string file, string username);

	/**
	 * Envia uma mensagem de texto para um determinado amigo.
	 *
	 * @param username Identificador o amigo que irá receber os arquivos
	 * @param text Mensagem a ser enviada
	 */
	void sendChatMessage(string username, string text);

	/**
	 * Retorna a lista de amigos conectados.
	 *
	 * @return Um vetor com o status dos amigos conectados.
	 * completo.
	 */
	vector<buzz::Status>* getFriends();

	/**
	 * Retorna a lista de amigos conectados.
	 *
	 * @return Um mapa que relaciona o identificador do amigo (jid completo em
	 * forma de string) com seu status completo.
	 */
	map<string, buzz::Status>* getFriendsMap();

	/**
	 * Permite definir o próprio status. Pode ser chamado antes ou depois de se
	 * conectar.
	 *
	 * @param s Status a ser utilizado. @see Status.
	 * @param message Mensagem personalizada opcional.
	 */
	void setStatus(Status s, string mensagem);

	/**
	 * Permite acessar o próprio status. Pode ser chamado antes ou depois de se
	 * conectar.
	 *
	 * @return Status completo.
	 */
	buzz::Status* getStatus();

	/**
	 * Aceita o recebimento de um arquivo.
	 */
	void accept();

	/**
	 * Rejeita o recebimento de um arquivo.
	 */
	void reject();

};
}
}
}
}
}
}
}

#endif /*P2PMANAGER_H*/
