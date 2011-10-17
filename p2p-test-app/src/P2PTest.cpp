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
 * @file P2PTest.cpp
 * @author Diogo de Carvalho Pedrosa
 * @author José Augusto Costa Martins Júnior
 * @author Armando Biagioni Neto
 * @date 03-09-10
 */

#include "../include/P2PTest.h"

#include <talk/base/cryptstring.h>

#include <fstream>		// Para ifstream (abrir arquivo de configuração)
#include <dirent.h>		// Para DIR (listar arquivos em um diretório)

//#if HAVE_COMPSUPPORT
//#include "system/io/ILocalDeviceManager.h"
//#else
#include "../../gingacc-cpp/gingacc-system/include/io/interface/output/dfb/DFBSurface.h"
#include "../../gingacc-cpp/gingacc-system/include/io/interface/output/dfb/DFBWindow.h"
#include "../../gingacc-cpp/gingacc-system/include/io/LocalDeviceManager.h"
//#endif

#include "../../telemidia-util-cpp/include/Color.h"


//#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;

#include <mmi/IEnhancedInputManager.h>
using namespace ::br::ufscar::lince::ginga::core::mmi;
//#else
//#include <mmi/EnhancedInputManager.h>
//#endif


#include <ginga/system/io/interface/input/CodeMap.h>
using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#include <p2p/FileShareClient.h>

namespace br {
namespace usp {
namespace icmc {
namespace intermidia {
namespace ginga {
namespace p2ptest {

P2PTest* P2PTest::_instance = NULL;

P2PTest::P2PTest() :
		backgroundColor(new Color(20, 20, 20)),
		selectedItemColor(new Color("lightGray")),
		fontColor(new Color("white")),
		selectedItemFontColor(new Color("black")),
		red(new Color("red")),
		green(new Color("green")),
		yellow(new Color("yellow")),
		blue(new Color("blue")),
		destinationJid(""),
		destinationStatus(NULL),
		fileTransfered(""),
		files(NULL),
		friendsName(NULL),
		friendsIsFileShareCapable(NULL),
		gi(new GraphicInterface(fontColor)),
		gl(new GraphicList(gi, 0, 125, 720, 40, 10, backgroundColor,
				selectedItemColor, fontColor, selectedItemFontColor)),
		chatMessages(new list<string>()),
		p2p(NULL),
		visibleScreen(NONE) {
	logger = LoggerUtil::getLogger("ginga.p2ptest.P2PTest");
	LoggerUtil::configure();
	LoggerUtil_info(logger, "Construtor");

	registerListener();
}

P2PTest* P2PTest::getInstance() {
	if (_instance == NULL) {
		_instance = new P2PTest();
	}
	return _instance;
}

P2PTest::~P2PTest() {
	delete backgroundColor;
	delete selectedItemColor;
	delete fontColor;
	delete selectedItemFontColor;
	delete red;
	delete green;
	delete yellow;
	delete blue;

	if (files != NULL) {
		delete files;
	}

	delete gi;
	delete gl;

	delete chatMessages;
}

void P2PTest::registerListener() {
	LoggerUtil_info(logger, "registerListener()");

//#if HAVE_COMPSUPPORT
	static IComponentManager* cm = IComponentManager::getCMInstance();
	IEnhancedInputManager* eim  = ((EnhancedInputManagerCreator*)
			(cm->getObject("InputManager")))();
//#else
//	IEnhancedInputManager* eim = IEnhancedInputManager::getInstance();
//#endif

	// Pede para receber eventos de pressionamento, mas não de liberação de
	// teclas do controle remoto.
	eim->addInputEventListener(this);

	// Pede para receber eventos multimodais.
	eim->addMultimodalInputEventListener(this);
}

void P2PTest::showMainScreen() {
	LoggerUtil_info(logger, "showMainScreen()");

	if (state == buzz::XmppEngine::STATE_OPEN) {
		showConnected();
	} else {
		showDisconnected();
	}
}

void P2PTest::showDisconnected() {
	LoggerUtil_info(logger, "showDisconnected()");

	gi->clear();

	int y = 0;
	int x = 0;

	gi->drawString(x, y, "Você está desconectado");
	y += 40;

	stringstream temp;
	temp << "(" << statusToString(p2p->getStatus()->show()) << ")";
	gi->drawString(x, y, temp.str().c_str());

	gi->setColor(red);
	gi->fillRectangle(x + 430, y + 5, 40, 30);
	gi->setColor(fontColor);
	gi->drawString(x + 442, y, "1   Mudar status");
	y += 90;

	gi->setColor(backgroundColor);
	gi->fillRectangle(x, y - 5, 720, 410);
	gi->setColor(fontColor);
	gi->drawString(x + 5, y + 5, "Bem vindo!");

	y = 540;
	gi->setColor(green);
	gi->fillRectangle(x, y + 5, 40, 30);
	gi->setColor(yellow);
	gi->fillRectangle(x + 333, y + 5, 40, 30);
	gi->setColor(blue);
	gi->fillRectangle(x + 548, y + 5, 40, 30);
	gi->setColor(selectedItemFontColor);
	gi->drawString(x + 345, y, "3");
	gi->setColor(fontColor);
	gi->drawString(x + 12, y, "2   Conectar");
	gi->drawString(x + 345, y, "    Sair");
	gi->drawString(x + 560, y, "4   Ajuda");

	gi->show();
	visibleScreen = DISCONNECTED;
}

void P2PTest::showConnected() {
	LoggerUtil_info(logger, "showConnected()");

	fillVectorWithNamesOfFriends();
	gl->setItens(friendsName, friendsIsFileShareCapable,
			"/usr/local/etc/ginga/p2ptest/fileshare.png");

	gi->clear();

	int y = 0;
	int x = 0;

	stringstream temp;
	temp << p2p->getStatus()->jid().BareJid().Str();
	gi->drawString(x, y, temp.str().c_str());
	y += 40;
	temp.str("");
	temp << "(" << statusToString(p2p->getStatus()->show()) << ")";
	gi->drawString(x, y, temp.str().c_str());

	gi->setColor(red);
	gi->fillRectangle(x + 430, y + 5, 40, 30);
	gi->setColor(fontColor);
	gi->drawString(x + 442, y, "1   Mudar status");
	y += 40;
	gi->drawString(x, y,
			"Selecione amigo para enviar arquivo");

	y += 50;
	gi->setColor(backgroundColor);
	gi->fillRectangle(x, y - 5, 720, 410);

	gi->setColor(fontColor);

	if (friendsName == NULL || friendsName->size() == 0) {
		gi->drawString(x, y + 5, "Não há amigos conectados");
	} else {
		gl->draw();
	}

	y = 540;
	gi->setColor(green);
	gi->fillRectangle(x, y + 5, 40, 30);
	gi->setColor(yellow);
	gi->fillRectangle(x + 333, y + 5, 40, 30);
	gi->setColor(blue);
	gi->fillRectangle(x + 548, y + 5, 40, 30);
	gi->setColor(selectedItemFontColor);
	gi->drawString(x + 345, y, "3");
	gi->setColor(fontColor);
	gi->drawString(x + 12, y, "2   Desconectar");
	gi->drawString(x + 345, y, "    Sair");
	gi->drawString(x + 560, y, "4   Ajuda");

	gi->show();
	visibleScreen = CONNECTED;
	LoggerUtil_info(logger, "Fim de showConnected()");
}

void P2PTest::connect() {
	LoggerUtil_info(logger, "connect");

	ifstream p2pConfigFile("/usr/local/etc/ginga/p2ptest/p2p.conf");

	if (p2pConfigFile.is_open()) {
		string server;
		string port;
		string username;

		getline(p2pConfigFile, server);
		getline(p2pConfigFile, port);
		getline(p2pConfigFile, username);

		talk_base::InsecureCryptStringImpl pass;
		std::string& passStr = pass.password();
		getline(p2pConfigFile, passStr);

		p2pConfigFile.close();

		p2p->connect(server, atoi(port.c_str()), username, pass, this);
	} else {
		LoggerUtil_info(logger,
				"Não foi possível abrir arquivo de configuração");
	}

	LoggerUtil_info(logger, "Fim de connect");
}

void P2PTest::fillVectorWithFileNames(string folder) {
	LoggerUtil_info(logger, "fillVectorWithFileNames");

	struct dirent *ep;
	DIR *dp = opendir (folder.c_str());

	if (files != NULL) {
		delete files;
	}

	files = new vector<string>();

	if (dp != NULL) {
		//LoggerUtil_info(logger, "if (dp != NULL) ");
		while (ep = readdir (dp)) {
			files->push_back(ep->d_name);
		}

		gl->reset();
		gl->setItens(files);

		closedir (dp);
	} else {
		LoggerUtil_info(logger, "Couldn't open the directory");
	}
}

void P2PTest::showPossibleFilesToSend() {
	LoggerUtil_info(logger, "showPossibleFilesToSend");

	gi->clear();

	int y = 0;
	int x = 0;

	// TODO Usar esse texto quando puder enviar mais de um item.
//	gi->drawString(x, y, "Para enviar mais de um item, utilize o OK para");
//	y += 40;
//	gi->drawString(x, y, "selecioná-los e só em seguida aperte o VERDE.");
//	y += 40;
//	gi->drawString(x, y, "Aperte VOLTAR para ver a tela principal.");
//	y += 90;

	gi->drawString(x, y, "Selecione o item que deseja enviar ou");
	y += 40;
	gi->drawString(x, y, "aperte VOLTAR para ver a tela principal.");

	gl->draw();

	visibleScreen = FILES;
	gi->show();
}

void P2PTest::showFileshareCapabilityWarning() {
	LoggerUtil_info(logger, "showFileshareCapabilityWarning");

	gi->clear();

	int y = 0;
	int x = 0;

	gi->drawString(x, y, "O amigo selecionado não possui capacidade");
	y += 40;
	gi->drawString(x, y, "de compartilhar arquivos. Selecione apenas");
	y += 40;
	gi->drawString(x, y, "amigos marcados com o ícone");
	gi->drawImage(x + 490, y + 5,
			"/usr/local/etc/ginga/p2ptest/fileshare.png");
	y += 60;
	gi->drawString(x, y, "Aperte OK ou VOLTAR para ver a tela principal.");

	visibleScreen = POPUP;
	gi->show();
}

void P2PTest::showSuccess() {
	LoggerUtil_info(logger, "showSuccess");

	gi->clear();

	int y = 0;
	int x = 0;

	stringstream temp;

	// Testa se foi realizado um recebimento.
	if (destinationJid.compare("") == 0) {
		// Algum item foi recebido.

		if (isFolder(fileTransfered)) {
			// Recebeu um diretório

			fillVectorWithImageNames(fileTransfered);

			if (files->size() > 0) {
				// TODO Slide show
				temp << "A pasta " << fileTransfered;
				gi->drawString(x, y, temp.str().c_str());
				y += 40;
				gi->drawString(x, y,
						"foi recebida com sucesso e contém imagens!");
				y += 40;
				gi->drawString(x, y,
						"Aperte OK para vê-las ou");
				y += 40;
				gi->drawString(x, y,
						"VOLTAR para ver a tela principal.");

				filesIterator = files->begin();
			} else {
				temp << "A pasta " << fileTransfered;
				gi->drawString(x, y, temp.str().c_str());
				y += 40;
				gi->drawString(x, y, "foi recebida com sucesso!");
			}
		} else {
			// Recebeu um arquivo.

			if (isImage(fileTransfered)) {
				// Recebeu uma imagem. Mostra ela na tela.

				gi->drawString(x, y,
						"Você recebeu a seguinte imagem:");
				y += 40;

				// TODO Surface do ginga não permite redimensionar a imagem, por
				// isso a imagem pode estrapolar a tela e ficar por cima da mensagem
				// mostrada abaixo. Limitar a área em que a imagem pode ser mostrada
				// e avisar ao usuário que apenas parte da imagem está sendo
				// exibida.
				gi->drawImage(x, y, fileTransfered);
				y += 450;
			} else {
				// Recebeu um arquivo que não é uma imagem.
				temp << "O arquivo " << fileTransfered;
				gi->drawString(x, y, temp.str().c_str());
				y += 40;
				gi->drawString(x, y, "foi recebido com sucesso!");
			}
		}
	} else {
		// Algum item foi enviado.

		if (isFolder(fileTransfered)) {
			temp << "A pasta " << fileTransfered;
			gi->drawString(x, y, temp.str().c_str());
			y += 40;
			gi->drawString(x, y, "foi enviada com sucesso!");
		} else {
			temp << "O arquivo " << fileTransfered;
			gi->drawString(x, y, temp.str().c_str());
			y += 40;
			gi->drawString(x, y, "foi enviado com sucesso!");
		}
	}

	if (destinationJid.compare("") == 0 && isFolder(fileTransfered) &&
			files->size() > 0) {
		visibleScreen = SLIDESHOW;
	} else {
		y += 40;
		gi->drawString(x, y, "Aperte OK ou VOLTAR para ver a tela principal.");

		visibleScreen = POPUP;
	}

	gi->show();
}

void P2PTest::fillVectorWithImageNames(string folder) {
	LoggerUtil_info(logger, "getImages");

	if (files != NULL) {
		delete files;
	}
	files = new vector<string>();

	struct dirent *ep;
	DIR *dp = opendir (folder.c_str());

	if (dp != NULL) {
		while (ep = readdir (dp)) {
			if (isImage(ep->d_name)) {
				stringstream folderPlusFile;
				folderPlusFile << folder << ep->d_name;
				files->push_back(folderPlusFile.str());
			}
		}
		closedir (dp);
	} else {
		LoggerUtil_info(logger, "Couldn't open the directory");
	}
}

bool P2PTest::isImage(string path) {
	LoggerUtil_info(logger, "isImage");

	if (path.find(".png") !=string::npos ||
			path.find(".bmp") !=string::npos ||
			path.find(".gif") !=string::npos ||
			path.find(".jpg") !=string::npos ) {
		return true;
	} else {
		return false;
	}
}

bool P2PTest::isFolder(string path) {
	DIR *dp = opendir (path.c_str());

	if (dp != NULL) {
		closedir(dp);
		return true;
	}
	return false;
}

void P2PTest::showImage() {
	LoggerUtil_info(logger, "showImage");

	gi->clear();

	int y = 0;
	int x = 0;

	gi->drawString(x, y, filesIterator->c_str());
	y += 40;

	// TODO Surface do ginga não permite redimensionar a imagem, por
	// isso a imagem pode estrapolar a tela e ficar por cima da mensagem
	// mostrada abaixo. Limitar a área em que a imagem pode ser mostrada
	// e avisar ao usuário que apenas parte da imagem está sendo
	// exibida.
	gi->drawImage(x, y, filesIterator->c_str());
	filesIterator++;
	y += 490;
	gi->drawString(x, y, "OK -> próxima, VOLTAR -> tela principal.");

	gi->show();
}

void P2PTest::showTransfering() {
	LoggerUtil_info(logger, "showTransfering");

	gi->clear();

	int y = 0;
	int x = 0;

	if (isFolder(fileTransfered)) {
		gi->drawString(x, y, "Aguarde enquanto a pasta");
		y += 40;
		gi->drawString(x, y, fileTransfered.c_str());
		y += 40;
		if (destinationJid.compare("") == 0) {
			gi->drawString(x, y, "está sendo recebida.");
		} else {
			gi->drawString(x, y, "está sendo enviada.");
		}
	} else {
		gi->drawString(x, y, "Aguarde enquanto o arquivo");
		y += 40;
		gi->drawString(x, y, fileTransfered.c_str());
		y += 40;
		if (destinationJid.compare("") == 0) {
			gi->drawString(x, y, "está sendo recebido.");
		} else {
			gi->drawString(x, y, "está sendo enviado.");
		}
	}

	visibleScreen = TRANSFERING;
	gi->show();
}

void P2PTest::showFriend(string* filename) {
	LoggerUtil_info(logger, "showFriend");

	gi->clear();

	int y = 0;
	int x = 0;

	if (destinationStatus->fileshare_capability()) {
		gi->drawImage(x, y + 5,
				"/usr/local/etc/ginga/p2ptest/fileshare.png");
		gi->drawString(x + 35, y,
				destinationStatus->jid().BareJid().Str().c_str());
	} else {
		gi->drawString(x, y,
				destinationStatus->jid().BareJid().Str().c_str());
	}

	if (destinationStatus->status().compare("") != 0) {
		y += 40;
		gi->drawString(x, y, destinationStatus->status().c_str());
	}

	y += 40;
	stringstream temp;
	temp << "(" << statusToString(destinationStatus->show()) << ")";
	gi->drawString(x, y, temp.str().c_str());

	y += 40;
	gi->drawString(x, y,
			"OK -> mandar \"Oi!\"   VOLTAR -> tela principal");

	y += 50;

	gi->setColor(backgroundColor);
	gi->fillRectangle(x, y - 5, 720, 410);
	gi->setColor(fontColor);

	if (filename != NULL) {
		gi->drawImage(x, y - 5, *filename);
	}

	list<string>::iterator iter = chatMessages->begin();
	while (iter != chatMessages->end()) {
		gi->drawString(x, y, iter->c_str());
		y += 40;
		iter++;
	}

	if (destinationStatus->fileshare_capability()) {
		y = 540;
		gi->setColor(green);
		gi->fillRectangle(x, y + 5, 40, 30);
		gi->setColor(fontColor);
		gi->drawString(x + 12, y, "2   Enviar arquivo");
	}

	gi->show();
}

void P2PTest::showHelp() {
	LoggerUtil_info(logger, "showHelp");

	gi->clear();

	int y = 0;
	int x = 0;

	gi->drawString(x, y, "Esta é a versão 1 da aplicação de teste do");
	y += 30;
	gi->drawString(x, y, "Componente P2P! Seu objetivo é demonstrar");
	y += 30;
	gi->drawString(x, y, "as funcionalidades providas por ele:");
	y += 35;
	gi->drawString(x, y, "- (Des)Conectar-se de/a um servidor XMPP;");
	y += 30;
	gi->drawString(x, y, "- Vizualizar os amigos conectados e seus status;");
	y += 30;
	gi->drawString(x, y, "- Enviar/receber arquivos e pastas a/de amigos;");
	y += 30;
	gi->drawString(x, y, "- Definir o prórpio status.");
	y += 35;
	gi->drawString(x, y, "Além disso, a aplicação permite vizualizar as");
	y += 30;
	gi->drawString(x, y, "imagens por ventura recebidas.");
	y += 35;
	gi->drawString(x, y, "O ícone      ao lado do nome de um amigo");
	gi->drawImage(x + 130, y + 5,
			"/usr/local/etc/ginga/p2ptest/fileshare.png");
	y += 30;
	gi->drawString(x, y, "indica capacidade de compartilhar arquivos.");
	y += 35;
	gi->drawString(x, y, "Usuários sem controle remoto podem usar");
	y += 30;
	gi->drawString(x, y, "as teclas equivalentes do teclado:");
	y += 30;
	gi->drawString(x, y, "ENTER = OK, ESC = VOLTAR, F1 = VERMELHO...");
	y += 35;
	gi->drawString(x, y, "A versão 2 permitirá ainda enviar/receber");
	y += 30;
	gi->drawString(x, y, "mensagens de texto a/de amigos.");
	y += 40;
	gi->drawString(x, y, "Aperte OK ou VOLTAR para ver a tela principal.");

	gi->show();
	visibleScreen = POPUP;
}

void P2PTest::showStatusChoice() {
	LoggerUtil_info(logger, "showStatusChoice");

	gi->clear();

	int y = 0;
	int x = 0;

	gi->drawString(x, y,
			"Escolha uma das opções de status abaixo:");
	y += 40;
	gi->drawString(x, y, "1 - Querendo conversar");
	y += 40;
	gi->drawString(x, y, "2 - Disponível");
	y += 40;
	gi->drawString(x, y, "3 - Ocupado");
	y += 40;
	gi->drawString(x, y, "4 - Ausente");
	y += 40;
	gi->drawString(x, y, "5 - Ausente por muito tempo");
	y += 60;

	gi->drawString(x, y,
			"ou aperte OK ou VOLTAR para ver a tela");
	y += 40;
	gi->drawString(x, y, "principal.");

	gi->show();
	visibleScreen = STATUS;
}

string P2PTest::statusToString(buzz::Status::Show statusShow) {
	switch (statusShow) {
		case buzz::Status::SHOW_XA: {
			return "Ausente por muito tempo";
		}
		case buzz::Status::SHOW_ONLINE: {
			return "Disponível";
		}
		case buzz::Status::SHOW_AWAY: {
			return "Ausente";
		}
		case buzz::Status::SHOW_DND: {
			return "Ocupado";
		}
		case buzz::Status::SHOW_CHAT: {
			return"Querendo conversar";
		}
		default: {
			return "Desconectado";
		}
	}
}

void P2PTest::stateChanged(buzz::XmppEngine::State state) {
	P2PTest* p2pTest = P2PTest::getInstance();
	LoggerUtil_info(p2pTest->logger, "stateChanged");

	p2pTest->state = state;

	// O P2PManager notifica usando STATE_NONE quando a mudança não é no estado
	// e sim no status do próprio usuário.
	if (state == buzz::XmppEngine::STATE_OPEN ||
			state == buzz::XmppEngine::STATE_CLOSED ||
			state == buzz::XmppEngine::STATE_NONE) {
		p2pTest->showMainScreen();
	}

	LoggerUtil_info(p2pTest->logger, "Fim de stateChanged");
}

void P2PTest::friendsStatusChanged(const buzz::Status &status) {
	P2PTest* p2pTest = P2PTest::getInstance();

	LoggerUtil_info(p2pTest->logger, "friendsStatusChanged");

	if (p2pTest->visibleScreen == CONNECTED) {
		p2pTest->showConnected();
	}

	LoggerUtil_info(p2pTest->logger, "Fim de friendsStatusChanged");
}

void P2PTest::chatMessageReceived(const string& from, const string& text) {
	P2PTest* p2pTest = P2PTest::getInstance();

	LoggerUtil_info(p2pTest->logger, "chatMessageReceived");

	stringstream temp;
	temp << from << ": ";

	p2pTest->chatMessages->push_back(temp.str());
	p2pTest->chatMessages->push_back(text);

	while (p2pTest->chatMessages->size() > 10) {
		p2pTest->chatMessages->pop_front();
	}

	// TODO Ir para chat se tiver na tela inicial tb.
	if (p2pTest->visibleScreen == FRIEND) {
		p2pTest->showFriend();
	}
}

void P2PTest::fillVectorWithNamesOfFriends() {
	LoggerUtil_info(logger, "fillVectorWithFriensNames");

	vector<buzz::Status>* friends = p2p->getFriends();

	if (friendsName != NULL) {
		delete friendsName;
	}
	friendsName = new vector<string>();

	if (friendsIsFileShareCapable != NULL) {
		delete friendsIsFileShareCapable;
	}
	friendsIsFileShareCapable = new vector<bool>();

	stringstream temp;

	vector<buzz::Status>::iterator iter = friends->begin();
	while (iter != friends->end()) {
		temp.str("");
		temp << iter->jid().BareJid().Str() << " - " <<
				statusToString(iter->show());
		friendsName->push_back(temp.str());

		if (iter->fileshare_capability()) {
			friendsIsFileShareCapable->push_back(true);
		} else {
			friendsIsFileShareCapable->push_back(false);
		}
		iter++;
	}
}

void P2PTest::transferStateChanged(FileShareClient::State state,
		const cricket::FileShareManifest* manifest) {

	P2PTest* p2pTest = P2PTest::getInstance();
	LoggerUtil_info(p2pTest->logger, "fileListener");

	switch (state) {
		case FileShareClient::OFFER: {
			// TODO Tratar o caso em que o usuário estava no processo de envio
			// de um arquivo.

			stringstream manifestDescription;

			// TODO Como fica a descrição se houver imagens (GetImageCount())???
			// TODO Mensagens da tela de aguarde e sucesso ficam sem sentido
			// quando mais de um item está sendo recebido.
			if (manifest->size() == 1) {
				manifestDescription <<  manifest->item(0).name;
				p2pTest->fileTransfered = manifest->item(0).name;
			} else if (manifest->GetFileCount() && manifest->GetFolderCount()) {
				manifestDescription <<  manifest->GetFileCount() <<
						" arquivos e " << manifest->GetFolderCount() <<
						" pastas";
			} else if (manifest->GetFileCount() > 0) {
				manifestDescription <<  manifest->GetFileCount() << " arquivos";
			} else {
				manifestDescription <<  manifest->GetFolderCount() << " pastas";
			}

			size_t filesize;

			// TODO session não está acessível daqui e por isso não podemos usar
			// o método GetTotalSize.
//			if (!session_->GetTotalSize(filesize)) {
//				manifestDescription << " (Tamanho desconhecido)";
//			} else {
//				manifestDescription << " (" << p2p->filesizeToString(filesize)
//						<< ")";
//			}

			p2pTest->showOffer(manifestDescription.str());
			break;
		}
		case FileShareClient::TRANSFER: {
			// TODO Mostrar barra de progresso?
			break;
		}
		case FileShareClient::COMPLETE: {
			p2pTest->showSuccess();

			p2pTest->fileTransfered = "";

			// Se tiver feito um envio, apaga os atributos destination.
			if (p2pTest->destinationJid.compare("") != 0) {
				p2pTest->destinationJid = "";
				p2pTest->destinationStatus = NULL;
			}
			break;
		}
		case FileShareClient::LOCAL_CANCEL:
		case FileShareClient::REMOTE_CANCEL:
		case FileShareClient::FAILURE:{
			// TODO Mostrar alguma coisa.
			p2pTest->showConnected();
			break;
		}
	}

	LoggerUtil_info(p2pTest->logger, "Fim de fileListener");
}


bool P2PTest::showOffer(string message) {
	LoggerUtil_info(logger, "showOffer");

	gi->clear();

	// A cor padrão do gi é branco, mas temos que setar aqui para
	// o segundo oferecimento de arquivo não ficar com letras verdes.
	gi->setColor(fontColor);

	int y = 0;
	int x = 0;
	gi->drawString(x, y, "Alguém está oferecendo");
	y += 40;
	string tmp = "\"";
	tmp.append(message).append("\".");
	gi->drawString(x, y, tmp.c_str());
	y += 40;
	gi->drawString(x, y, "Você aceita?");
	y += 40;
	gi->drawString(x, y, "       Não          Sim");

	gi->setColor(red);
	gi->fillRectangle(x, y + 4, 48, 28);

	gi->setColor(green);
	gi->fillRectangle(x + 160, y + 4, 48, 28);

	// Mantem a cor da surface branca para não ocorrer de escrever nada verde
	// em outra tela.
	gi->setColor(fontColor);

	gi->show();
	visibleScreen = ACCEPTING;

	LoggerUtil_info(logger, "Fim de showOffer");
}

bool P2PTest::multimodalUserEventReceived(IMultimodalInputEvent* ev) {
	LoggerUtil_info(logger, "multimodalUserEventReceived");

	switch (visibleScreen) {
		case FRIEND: {
			// Trata recebimento de strings.
			vector<string>* strs = ev->getStrings();
			string sendTo = destinationStatus->jid().BareJid().Str();
			stringstream temp;
			for (vector<string>::iterator j = strs->begin();
					j != strs->end(); j++) {
				p2p->sendChatMessage(sendTo, *j);
				temp.str("");
				temp << "Eu: " << *j;
				chatMessages->push_back(temp.str());
			}

			// Trata recebimento de arquivos binários.
			vector<File*>* files = ev->getBinaries();

			string filename = "";

			for (vector<File*>::iterator f = files->begin();
					f != files->end(); f++) {
				temp.str("");
				filename = (*f)->getName();
				temp << "Oferecendo arquivo " << filename;
				chatMessages->push_back(temp.str());
				p2p->sendFile(filename, sendTo);
			}

			// Faz rolagem, se necessário.
			while (chatMessages->size() > 10) {
				chatMessages->pop_front();
			}

			if (isImage(filename)) {
				// Repinta a tela.
				showFriend(&filename);
			} else {
				showFriend();
			}

			break;
		}
	}
}

bool P2PTest::userEventReceived(IInputEvent* ev) {
	LoggerUtil_info(logger, "userEventReceived(IInputEvent* ev)");

	const int code = ev->getKeyCode();

	switch (visibleScreen) {
		case NONE: {
			if (code == CodeMap::KEY_RED  || code == CodeMap::KEY_F1 ||
					code == CodeMap::KEY_1) {
				// O objeto P2PManager deve obrigatóriamente ser criado na mesma
				// thread em que os métodos dele serão chamados. Por isso que
				// ele está sendo criado aqui e não no construtor.
				if (p2p == NULL) {
					p2p = new P2PManager();
				}
				showMainScreen();
			}
			break;
		}
		case DISCONNECTED: {
			// TODO error: ‘br::pucrio::telemidia::ginga::core::system::io::
			// CodeMap::KEY_1’ cannot appear in a constant-expression
//		    switch (code) {
//				case CodeMap::KEY_1: {
//				...
//			}
			if (code == CodeMap::KEY_1 || code == CodeMap::KEY_RED  ||
					code == CodeMap::KEY_F1) {
				showStatusChoice();
			} else if (code == CodeMap::KEY_2 || code == CodeMap::KEY_GREEN  ||
					code == CodeMap::KEY_F2) {
				connect();
				showConnected();
			} else if (code == CodeMap::KEY_3 || code == CodeMap::KEY_YELLOW  ||
					code == CodeMap::KEY_F3) {
				p2p->disconnect();
				gi->clear();
				gi->show();
				visibleScreen = NONE;
			} else if (code == CodeMap::KEY_4 || code == CodeMap::KEY_BLUE  ||
					code == CodeMap::KEY_F4 || code == CodeMap::KEY_INFO) {
				showHelp();
			}
			break;
		}
		case CONNECTED: {
			if (code == CodeMap::KEY_1 || code == CodeMap::KEY_RED  ||
					code == CodeMap::KEY_F1) {
				showStatusChoice();
			} else if (code == CodeMap::KEY_2 || code == CodeMap::KEY_GREEN  ||
					code == CodeMap::KEY_F2) {
				p2p->disconnect();
				showDisconnected();
			} else if (code == CodeMap::KEY_3 || code == CodeMap::KEY_YELLOW  ||
					code == CodeMap::KEY_F3) {
				p2p->disconnect();
				gi->clear();
				gi->show();
				visibleScreen = NONE;
			} else if (code == CodeMap::KEY_4 || code == CodeMap::KEY_BLUE  ||
					code == CodeMap::KEY_F4 || code == CodeMap::KEY_INFO) {
				showHelp();
			} else if (code == CodeMap::KEY_CURSOR_UP) {
				if (gl->upPressed()) {
					showConnected();
				}
			} else if (code == CodeMap::KEY_CURSOR_DOWN) {
				if (gl->downPressed()) {
					showConnected();
				}
			} else if (code == CodeMap::KEY_OK || code == CodeMap::KEY_ENTER) {
				int index = gl->getFocusedItem();
				destinationStatus = &(p2p->getFriends()->at(index));

				chatMessages->clear();
				visibleScreen = FRIEND;
				showFriend();
			}
			break;
		}
		case ACCEPTING: {
			if (code == CodeMap::KEY_1 || code == CodeMap::KEY_2 ||
					code == CodeMap::KEY_RED || code == CodeMap::KEY_GREEN ||
					code == CodeMap::KEY_F1 || code == CodeMap::KEY_F2) {

				showTransfering();

				if (code == CodeMap::KEY_GREEN || code == CodeMap::KEY_F2 ||
						code == CodeMap::KEY_2) {
					p2p->accept();
				} else {
					p2p->reject();
				}
			}
			break;
		}
		case POPUP: {
			if (code == CodeMap::KEY_OK || code == CodeMap::KEY_ENTER ||
					code == CodeMap::KEY_BACK || code == CodeMap::KEY_ESCAPE) {
				showMainScreen();
			}
			break;
		}
		case STATUS: {
			if (code >= CodeMap::KEY_1 && code <= CodeMap::KEY_5) {
				IP2PManager::Status s;

				if (code == CodeMap::KEY_1) {
					s = IP2PManager::CHAT;
				} else if (code == CodeMap::KEY_2) {
					s  = IP2PManager::ONLINE;
				} else if (code == CodeMap::KEY_3) {
					s = IP2PManager::DND;
				} else if (code == CodeMap::KEY_4) {
					s = IP2PManager::AWAY;
				} else if (code == CodeMap::KEY_5) {
					s = IP2PManager::XA;
				}

				string mensagem = "";
//				cout << "Digite uma mensagem personalizada (opcional):" << endl;
//				cin >> mensagem;

				p2p->setStatus(s, mensagem);
				showMainScreen();
			} else if (code == CodeMap::KEY_OK || code == CodeMap::KEY_ENTER ||
					code == CodeMap::KEY_BACK || code == CodeMap::KEY_ESCAPE) {
				showMainScreen();
			}

			break;
		}
		case FILES: {
			if (code == CodeMap::KEY_BACK || code == CodeMap::KEY_ESCAPE) {
				showConnected();
			} else if (code == CodeMap::KEY_OK || code == CodeMap::KEY_ENTER) {
				fileTransfered = gl->getFocusedItemString();
				showTransfering();
				p2p->sendFile(fileTransfered, destinationJid);
			} else if (code == CodeMap::KEY_CURSOR_UP) {
				if (gl->upPressed()) {
					showPossibleFilesToSend();
				}
			} else if (code == CodeMap::KEY_CURSOR_DOWN) {
				if (gl->downPressed()) {
					showPossibleFilesToSend();
				}
			}
			
			break;
		}
		case FRIEND: {
			if (code == CodeMap::KEY_BACK || code == CodeMap::KEY_ESCAPE) {
				showConnected();
			} else if (code == CodeMap::KEY_GREEN || code == CodeMap::KEY_F2 ||
					code == CodeMap::KEY_2) {
				if (destinationStatus->fileshare_capability()) {
					destinationJid = destinationStatus->jid().BareJid().Str();
					// Prepara o vetor files para ser usado pela função
					// showPossibleFilesToSend.
					fillVectorWithFileNames("./");
					showPossibleFilesToSend();
				} else {
					showFileshareCapabilityWarning();
				}
			} else if (code == CodeMap::KEY_OK || code == CodeMap::KEY_ENTER) {
				p2p->sendChatMessage(destinationStatus->jid().BareJid().Str(),
						"Oi!");
				chatMessages->push_back("Eu: Oi!");

				if (chatMessages->size() > 10) {
					chatMessages->pop_front();
				}

				showFriend();
			}
			break;
		}
		case SLIDESHOW: {
			if (code == CodeMap::KEY_OK || code == CodeMap::KEY_ENTER) {
				if (filesIterator != files->end()) {
					showImage();
				} else {
					showConnected();
				}
			} else if (code == CodeMap::KEY_BACK || code == CodeMap::KEY_ESCAPE) {
				showConnected();
			}
			break;
		}
	}

	return true;
}
}
}
}
}
}
}
