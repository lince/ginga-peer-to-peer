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
 * @file FileShareClient.h
 * @author Armando Biagioni Neto
 * @author Diogo de Carvalho Pedrosa
 * @author José Augusto Costa Martins Júnior
 * @date 22-07-10
 */

#ifndef FILESHARECLIENTE_H
#define FILESHARECLIENTE_H

#include <talk/session/fileshare/fileshare.h>	// Para cricket::FileShareManifest
#include <talk/examples/login/status.h>			// Para buzz::Status
#include <talk/xmpp/jid.h>						// Para buzz::Jid

#include <linceutil/LoggerUtil.h>
using namespace ::br::ufscar::lince::util;

namespace br {
namespace usp {
namespace icmc {
namespace intermidia {
namespace ginga {
namespace core {
namespace p2p {

// Faço essa declaração aqui em vez de fazer um include para evitar include
// cíclicos.
class IP2PEventListener;

/**
 * Classe responsável pela troca de arquivos entre clientes XMPP.
 */
class FileShareClient : public sigslot::has_slots<>,
public talk_base::MessageHandler {
public:
	enum State {
		OFFER			= 0, /*!< Alguem está lhe oferecendo arquivos (acompanha um manifesto). */
		TRANSFER		= 1, /*!< Tranferência iniciada. */
		COMPLETE		= 2, /*!< Transferência completada. */
		LOCAL_CANCEL	= 3, /*!< Transferência cancelada localmente. */
		REMOTE_CANCEL	= 4, /*!< Transferência cancelada remotamente. */
		FAILURE			= 5, /*!< Falha durante transferência. */
	};

private:
	talk_base::scoped_ptr<cricket::FileShareSessionClient>
			file_share_session_client_;
	buzz::Jid send_to_jid_;
	const cricket::FileShareManifest *manifest_;
	cricket::FileShareSession *session_;
	std::string root_dir_;

	/// Observador registrado para ser notificado do recebimento de um arquivo.
	IP2PEventListener* listener;

	/// Componente auxiliar para realização de logs.
	HLoggerPtr logger;

	void OnMessage(talk_base::Message *m);

	std::string filesizeToString(unsigned int size);

	void OnSessionState(cricket::FileShareState state);

	void OnUpdateProgress(cricket::FileShareSession *sess);

	void OnResampleImage(std::string path, int width, int height,
			talk_base::HttpTransaction *trans);

	void OnFileShareSessionCreate(cricket::FileShareSession *sess);

	unsigned int getDirSize(const char *directory);


public:
	/**
	 * Constrói um objeto FileShareClient já passando tudo que é necessário
	 * para realizar uma conexão.
	 *
	 * @param root_dir Diretório onde os arquivos serão procurados ou escritos.
	 */
	FileShareClient(std::string root_dir);

	/**
	 * Destrói um objeto FileShareClient.
	 */
	~FileShareClient();

	/**
	 * Permite setar o objeto responsável por tratar mudanças de estado
	 * relacionadas a trasnferência de arquivos e pastas, inclusive o
	 * recebimento de oferecimento de um item.
	 *
	 * @param listener Observador que será notificado quando ocorrer qualquer
	 * mudança de estado relacionada a transferência de algum arquivo.
	 */
	void setListener(IP2PEventListener* listener);

	/**
	 * Permite definir o próprio status.
	 *
	 * @param show Status a ser utilizado.
	 * @param message Mensagem personalizada opcional.
	 */
	void setStatus(buzz::Status::Show show, string message);

	/**
	 * Envia arquivos e diretórios para um determinado amigo.
	 *
	 * @param files Vetor contendo os caminhos dos arquivos e diretórios que
	 * serão enviados
	 * @param jid Identificador o amigo que irá receber os arquivos
	 */
	void sendFiles(vector<string>* files, const buzz::Jid &send_to,
			map<string, buzz::Status>* roster);

	/**
	 * Método chamado depois que uma conexão é estabelecida. Envia informação de
	 * presença e se registra para receber notificações de presença de outros.
	 *
	 * @param session_manager O gerenciador da sessão atual.
	 * @param jid O endereço XMPP do usuário.
	 */
	void onSignon(cricket::SessionManager* session_manager,
			const buzz::Jid jid);

	/**
	 * Método chamado quando algum usuário fica online, para que esta classe
	 * possa checar se existe algum item que está pendente para ser enviado para
	 * esse usuário.
	 *
	 * @param status Informações sobre o usuário que ficou online.
	 */
	void onPossibleDestinationOnline(const buzz::Status &status);

	/**
	 * Possíveis valores para mensagens a serem trocadas entre threads através
	 * da MessageQueue.
	 */
	enum {
		MSG_STOP,
		MSG_DECLINE,
		MSG_ACCEPT,
	};
};

}
}
}
}
}
}
}

#endif /*FILESHARECLIENTE_H*/
