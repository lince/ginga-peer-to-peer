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
Maria da Graça Pimentel - mgp@icmc.usp.br
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
http://www.ncl.org.br
http://www.ginga.org.br
http://agua.intermidia.icmc.usp.br/intermidia
 *******************************************************************************/

/**
 * @file P2PManager.cpp
 * @author Armando Biagioni Neto
 * @author Diogo de Carvalho Pedrosa
 * @author José Augusto Costa Martins Júnior
 * @date 22-07-10
 */

#include "../include/P2PManager.h"

#include "../config.h"

#if HAVE_COMPSUPPORT
#include <cm/IComponentManager.h>
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "../include/PurpleP2PConnection.h"
#endif

#include <iomanip>					// Para setprecision

#include <talk/base/ssladapter.h>	// Para talk_base::InitializeSSL()
#include <talk/base/helpers.h>		// Para initRandom (OnSignon)
#include <talk/examples/login/presencepushtask.h>	// Para buzz::PresencePushTask
#include <talk/examples/login/presenceouttask.h>	// Para buzz::PresenceOutTask
#include <talk/examples/login/jingleinfotask.h>		// Para buzz::JingleInfoTask
#include <talk/p2p/client/sessionmanagertask.h>		// Para cricket::SessionManagerTask

namespace br {
namespace usp {
namespace icmc {
namespace intermidia {
namespace ginga {
namespace core {
namespace p2p {
#if HAVE_COMPSUPPORT
static IComponentManager* cm = IComponentManager::getCMInstance();
#endif

P2PManager::P2PManager() : pump(new XmppPump),
		main_thread(NULL),
		fs_client(NULL),
		chatTask(NULL),
		xcs(NULL),
		xmppSocket(NULL),
		roster_(new map<string, buzz::Status>),
		rosterVector_(NULL),
		my_status(new buzz::Status),
		listener(NULL),
		connected(false) {

	logger = LoggerUtil::getLogger("ginga.core.p2p.P2PManager");
	LoggerUtil::configure();
	LoggerUtil_info(logger, "Construtor");

	// O valor padrão quando o log está habilitado é LS_INFO. Os valores
	// possíveis são:
	// LS_SENSITIVE, LS_VERBOSE, LS_INFO, LS_WARNING, LS_ERROR.
	// Explicação breve sobre cada um pode ser encontrado em base/logging.h
	// NO_LOGGING está definido dentro da classe LogMessage como sendo
	// LS_ERROR + 1, portanto, as duas formas abaixo são possíveis.
	//		talk_base::LogMessage::LogToDebug(talk_base::LS_ERROR + 1);
	//		talk_base::LogMessage::LogToDebug(talk_base::LogMessage::NO_LOGGING);
	// Aqui, definimos que queremos apenas mensagens de erro.
	//		talk_base::LogMessage::LogToDebug(talk_base::LS_ERROR);

	// Inicializa o SSL. Método definido em base/ssladapter.cc
	if (!talk_base::InitializeSSL()) {
		LoggerUtil_info(logger, "Algum erro ocorreu durante a inicialização"
				"do SSL");
	}

	// Status começa em SHOW_NONE, por isso mudamos para online.
	my_status->set_show(buzz::Status::SHOW_ONLINE);

	// Define parte dos valores de um XmppClientSettings
	xcs = new buzz::XmppClientSettings();
	xcs->set_resource("Ginga"); // Usado como parte do Jid
	xcs->set_use_tls(true);

	// Cria uma nova thread para sinalização (signaling thread) e a
	// define como ativa. Essa mesma thread está sendo usada como worker
	// thread também. Não é necessário criar um PhysicalSocketServer
	// porque ele é criado automaticamente pela classe MessageQueue, pai
	// da classe Thread. Nesse caso o ss será deletado automaticamente
	// tb. Caso se deseje que o ss viva mais que a thread, deve-se criá-lo
	// antes e passá-lo como parâmetro.
	//			ss = new talk_base::PhysicalSocketServer();
	//			main_thread = new talk_base::Thread(ss);
	main_thread = new talk_base::Thread();
	talk_base::ThreadManager::SetCurrent(main_thread);

	// Usado pelo FileShareClient para saber onde gravar os arquivos
	// recebidos e talvez onde procurar os arquivos a serem enviados.
	// Função declarada em unistd.h.
	getcwd(currentWorkingDirectory, sizeof(currentWorkingDirectory));

	// Cria a instância do FileShareClient
	fs_client = new FileShareClient(currentWorkingDirectory);

	// Define que a função OnStateChange do cliente de compartilhamento
	// de arquivo deve ser chamada quando algum evento de mudança de
	// estado ocorrer.
	pump->client()->SignalStateChange.connect(this, &P2PManager::OnStateChange);
}

P2PManager::~P2PManager() {
	LoggerUtil_info(logger, "Destrutor");

	// Finaliza o SSL. Método definido em base/ssladapter.cc
	if (!talk_base::CleanupSSL()) {
		LoggerUtil_info(logger, "Algum erro ocorreu durante a finalização"
				"do SSL");
	}

	if (chatTask != NULL) {
		delete chatTask;
	}

	if (pump != NULL) {
		delete pump;
		pump = NULL;
	}

	if (main_thread != NULL) {
		delete main_thread;
		main_thread = NULL;
	}

	delete xcs;
	xcs = NULL;

	delete roster_;
	roster_ = NULL;

	delete my_status;
	my_status = NULL;

	if (rosterVector_ != NULL) {
		delete rosterVector_;
		rosterVector_ = NULL;
	}

	LoggerUtil_info(logger, "Fim do destrutor");
}


void P2PManager::OnStateChange(buzz::XmppEngine::State state) {
	LoggerUtil_info(logger, "OnStateChange");

	switch (state) {
		case buzz::XmppEngine::STATE_START:
			cout << "Connecting..." << endl;
			break;
		case buzz::XmppEngine::STATE_OPENING:
			cout << "Logging in. " << endl;
			break;
		case buzz::XmppEngine::STATE_OPEN:
//			cout << "Logged in as " << pump->client()->jid().Str() << endl;
			cout << "Logged in." << endl;
			onSignon();
			break;
		case buzz::XmppEngine::STATE_CLOSED:
			cout << "Logged out." << endl;
			break;
	}

	if (listener != NULL) {
		listener->stateChanged(state);
	}

	LoggerUtil_info(logger, "Fim de OnStateChange");
}

void P2PManager::connect(string server, int port, string username,
		talk_base::InsecureCryptStringImpl pass,
		IP2PEventListener* listener) {

	if (!connected) {
		LoggerUtil_info(logger, "connect(server, port, username, pass)");

		// Checa se o username passado possui um formato válido.
		buzz::Jid jid = buzz::Jid(username);
		if (!jid.IsValid() || jid.node() == "") {
			LoggerUtil_info(logger,
					"Invalid JID. JIDs should be in the form user@domain");
			return;
		}

		// Define os valores de um XmppClientSettings
		xcs->set_user(jid.node());
		xcs->set_host(jid.domain());
		xcs->set_pass(talk_base::CryptString(pass));
		xcs->set_server(talk_base::SocketAddress(server, port));

		this->listener = listener;
		fs_client->setListener(listener);

		// Realiza o login e inicia a thread que irá esperar o recebimento
		// de arquivos.
		// DoLogin não faz nada até que Start seja chamado. Quando o login
		// tiver sido realizado, STATE_OPEN será enviado para o método
		// OnStateChange.
		xmppSocket  = new XmppSocket(true); // true -> usa TLS.
		pump->DoLogin(*xcs, xmppSocket, NULL);

		main_thread->Start();
		connected = true;

		LoggerUtil_info(logger, "Fim de connect");
	}
}

void P2PManager::disconnect() {
	LoggerUtil_info(logger, "disconnect");

	if (connected) {
		main_thread->Post(fs_client, FileShareClient::MSG_STOP);
		pump->DoDisconnect();

		connected = false;

		// Provavelmente está sendo deletado durante a deleção da session,
		// dentro do método OnMessage (STOP), pois se deletamos aqui, dá
		// erro de memória lá.
//		delete xmppSocket;
		xmppSocket = NULL;

		// Já tá sendo deletado por alguém, pois quando chamo delete, dá
		// erro de memória aqui.
//		delete fs_client;
		fs_client = NULL;

		delete main_thread;
		main_thread = NULL;

		// Depois que movi o método OnStateChange de FileShareClient para
		// esta classe, essa chamada delete passou a dar erro. Acredito que
		// a ordem do acontecimento de algumas coisas tenha mudado e agora,
		// neste ponto, o pump já foi deletado por alguém.
//		delete pump;
		pump = NULL;

		// Refaz alguns passos que estão sendo feitos no construtor para
		// permitir uma reconexão futura.
		pump = new XmppPump();
		main_thread = new talk_base::Thread();
		talk_base::ThreadManager::SetCurrent(main_thread);
		fs_client = new FileShareClient(currentWorkingDirectory);
		pump->client()->SignalStateChange.connect(this,
				&P2PManager::OnStateChange);
	}
	LoggerUtil_info(logger, "Fim de disconnect");
}

void P2PManager::sendFiles(vector<string>* files, string username) {
	LoggerUtil_info(logger, "sendFiles");

	// Checa se está conectado.
	if (!connected) {
		LoggerUtil_info(logger, "É necessário se conectar antes.");
		return;
	}

	// Checa se o username passado possui um formato válido.
	buzz::Jid sendToJid = buzz::Jid(username);
	if (!sendToJid.IsValid() || sendToJid.node() == "") {
		LoggerUtil_info(logger,
				"Invalid JID. JIDs should be in the form user@domain");
		return;
	}

	fs_client->sendFiles(files, sendToJid, roster_);

	LoggerUtil_info(logger, "Fim de sendFiles");
}

void P2PManager::sendFile(string file, string username) {
	LoggerUtil_info(logger, "sendFile");

	vector<string>* temp = new vector<string>();
	temp->push_back(file);
	sendFiles(temp, username);
	delete temp;

	LoggerUtil_info(logger, "Fim de sendFile");
}

void P2PManager::sendChatMessage(string username, string text) {
	LoggerUtil_info(logger, "sendMessage");

	// Checa se está conectado.
	if (!connected) {
		LoggerUtil_info(logger, "É necessário se conectar antes.");
		return;
	}

	// Checa se o username passado possui um formato válido.
	buzz::Jid sendToJid = buzz::Jid(username);
	if (!sendToJid.IsValid() || sendToJid.node() == "") {
		LoggerUtil_info(logger,
				"Invalid JID. JIDs should be in the form user@domain");
		return;
	}

	chatTask->sendChatMessage(&sendToJid, text);

	LoggerUtil_info(logger, "Fim de sendMessage");
}

vector<buzz::Status>* P2PManager::getFriends() {
	LoggerUtil_info(logger, "getFriends");

	// Checa se está conectado.
	if (!connected) {
		LoggerUtil_info(logger, "É necessário se conectar antes.");
		return NULL;
	}

	if (rosterVector_ != NULL) {
		delete rosterVector_;
	}

	rosterVector_ = new vector<buzz::Status>();

	map<string, buzz::Status>::iterator iter = roster_->begin();

	while (iter != roster_->end()) {
		rosterVector_->push_back(iter->second);
		iter++;
	}

	return rosterVector_;
}

map<string, buzz::Status>* P2PManager::getFriendsMap() {
	LoggerUtil_info(logger, "getFriendsMap");

	// Checa se está conectado.
	if (!connected) {
		LoggerUtil_info(logger, "É necessário se conectar antes.");
		return NULL;
	}

	return roster_;
}

void P2PManager::setStatus(Status s, string message) {
	LoggerUtil_info(logger, "setStatus");

	switch (s) {
		case CHAT: {
			my_status->set_show(buzz::Status::SHOW_CHAT);
			break;
		}
		case ONLINE: {
			my_status->set_show(buzz::Status::SHOW_ONLINE);
			break;
		}
		case DND: {
			my_status->set_show(buzz::Status::SHOW_DND);
			break;
		}
		case AWAY: {
			my_status->set_show(buzz::Status::SHOW_AWAY);
			break;
		}
		case XA: {
			my_status->set_show(buzz::Status::SHOW_XA);
			break;
		}
		default: {
			LoggerUtil_info(logger, "Opção invalida!");
			return;
		}
	}

	// Seta os novos valores do status atual.
	my_status->set_status(message);

	// Avisa ao servidor caso a conexão já esteja estabelecida.
	if (connected) {
		buzz::PresenceOutTask* presence_out_ =
				new buzz::PresenceOutTask(pump->client());
		presence_out_->Send(*my_status);
		presence_out_->Start();
	}
	LoggerUtil_info(logger, "Fim de setStatus");
}

buzz::Status* P2PManager::getStatus() {
	LoggerUtil_info(logger, "getStatus");
	return my_status;
}

void P2PManager::accept() {
	LoggerUtil_info(logger, "accept");
	main_thread->Post(fs_client, FileShareClient::MSG_ACCEPT);
}

void P2PManager::reject() {
	LoggerUtil_info(logger, "reject");
	main_thread->Post(fs_client, FileShareClient::MSG_DECLINE);
}

void P2PManager::OnJingleInfo(const std::string & relay_token,
		const std::vector<std::string> &relay_addresses,
		const std::vector<talk_base::SocketAddress> &stun_addresses) {
	LoggerUtil_info(logger, "OnJingleInfo");
	port_allocator_->SetStunHosts(stun_addresses);
	port_allocator_->SetRelayHosts(relay_addresses);
	port_allocator_->SetRelayToken(relay_token);
	LoggerUtil_info(logger, "Fim de OnJingleInfo");
}

void P2PManager::OnStatusUpdate(const buzz::Status &status) {
	LoggerUtil_info(logger, "OnStatusUpdate");

	string key = status.jid().Str();

	// Só faz alguma coisa se a notificação não for sobre mim mesmo (na mesma
	// conexão).
	if (status.jid().Compare(my_status->jid()) != 0) {
		if (status.available()) {
			if (status.fileshare_capability()) {
				fs_client->onPossibleDestinationOnline(status);
			}

			cout << key << " está online." << endl;
			(*roster_)[key] = status;
		} else {
			cout << key << " está offline." << endl;

			map<string, buzz::Status>::iterator iter = roster_->find(key);

			if (iter != roster_->end()) {
				roster_->erase(iter);
			}
		}

		listener->friendsStatusChanged(status);
	}

	LoggerUtil_info(logger, "Fim de OnStatusUpdate");
}

void P2PManager::onSignon() {
	LoggerUtil_info(logger, "onSignon");

	// Essas duas linhas são responsáveis por setar na biblioteca a semente
	// do gerador de valores aleatórios. Não sei onde o gerador está sendo
	// usado. Pode ser que seja para gerar o JID completo.
	std::string client_unique = pump->client()->jid().Str();
	cricket::InitRandom(client_unique.c_str(), client_unique.size());

	// Pede para receber notificações de mudanças de estado dos amigos
	// através do método OnStatusUpdate.
	buzz::PresencePushTask* presence_push_ =
			new buzz::PresencePushTask(pump->client());
	presence_push_->SignalStatusUpdate.connect(this,
			&P2PManager::OnStatusUpdate);
	presence_push_->Start();

	// Cria uma instância indicando meu status atual.
	my_status->set_jid(pump->client()->jid());
	my_status->set_available(true);
	my_status->set_priority(0);
	my_status->set_know_capabilities(true);
	my_status->set_fileshare_capability(true);
	my_status->set_is_google_client(true);
	my_status->set_version("1.0.0.66");

	// Avisa ao servidor qual o meu status atual.
	buzz::PresenceOutTask* presence_out_ =
			new buzz::PresenceOutTask(pump->client());
	presence_out_->Send(*my_status);
	presence_out_->Start();

	// Segundo parâmetro é o userAgent.
	port_allocator_.reset(new cricket::HttpPortAllocator(&network_manager_,
			"Ginga"));

	// O segundo parâmetro indica que não será usada uma worker thread separada.
	// O método reset deleta o objeto apontado e coloca um novo objeto no lugar.
	session_manager_.reset(new cricket::SessionManager(port_allocator_.get(),
			NULL));

	cricket::SessionManagerTask* session_manager_task = new
			cricket::SessionManagerTask(pump->client(), session_manager_.get());
	session_manager_task->EnableOutgoingMessages();
	session_manager_task->Start();

	buzz::JingleInfoTask* jingle_info_task =
			new buzz::JingleInfoTask(pump->client());
	jingle_info_task->RefreshJingleInfoNow();
	jingle_info_task->SignalJingleInfo.connect(this, &P2PManager::OnJingleInfo);
	jingle_info_task->Start();

	fs_client->onSignon(session_manager_.get(), pump->client()->jid());

	if (chatTask != NULL) {
		delete chatTask;
	}
	chatTask = new ChatTask(pump->client());
	chatTask->SignalIncomingChat.connect(this,
			&P2PManager::OnChatMessageReceived);

	LoggerUtil_info(logger, "Fim de onSignon");
}

void P2PManager::OnChatMessageReceived(buzz::Jid from,
		   const string& text) {
	LoggerUtil_info(logger, "OnIncomingChat");

	if (listener != NULL) {
		listener->chatMessageReceived(from.BareJid().Str(), text);
	}

	LoggerUtil_info(logger, "Fim de OnIncomingChat");
}

}
}
}
}
}
}
}


extern "C" ::br::usp::icmc::intermidia::ginga::core::p2p::IP2PManager*
createP2PManager() {

	return new ::br::usp::icmc::intermidia::ginga::core::p2p::P2PManager();
}

extern "C" void destroyP2PManager(
		::br::usp::icmc::intermidia::ginga::core::p2p::IP2PManager* p2pm) {

	delete p2pm;
	p2pm = NULL;
}
