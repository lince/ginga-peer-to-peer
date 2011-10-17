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
 * @file P2PTest.h
 * @author Diogo de Carvalho Pedrosa
 * @author José Augusto Costa Martins Júnior
 * @author Armando Biagioni Neto
 * @date 30-07-10
 */

#ifndef P2PTEST_H
#define P2PTEST_H

#include <linceutil/LoggerUtil.h>
using namespace ::br::ufscar::lince::util;

#include <iostream>
using namespace std;


// TODO Tirar esse include para deixar a app de teste independente da libjingle!
#include <talk/session/fileshare/fileshare.h>	// cricket::FileShareManifest
#include <talk/xmpp/xmppengine.h>				// Para buzz::XmppEngine::State

#include "system/io/interface/input/IInputEventListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#include "mmi/IMultimodalInputEventListener.h"
using namespace ::br::ufscar::lince::ginga::core::mmi;

#include <p2p/P2PManager.h>
using namespace ::br::usp::icmc::intermidia::ginga::core::p2p;

#include "GraphicInterface.h"
#include "GraphicList.h"


namespace br {
namespace usp {
namespace icmc {
namespace intermidia {
namespace ginga {
namespace p2ptest {

/**
 * Classe responsável por testar as funcionalidades do Compoente P2P.
 */
class P2PTest : public IInputEventListener,
		public IMultimodalInputEventListener, public IP2PEventListener {
protected:
	/**
	 * \enum
	 * Possíveis valores para definir o status do usuário.
	 */
	enum Screen {
		NONE,			/*!< Esperando para iniciar de verdade a aplicação */
		DISCONNECTED,	/*!< Tela principal enquanto desconectado */
		CONNECTED,		/*!< Tela principal enquanto conectado */
		ACCEPTING,		/*!< Esperando aceitação ou rejeição de arquivo */
		STATUS,			/*!< Permite que o usuário defina um novo status */
		POPUP, 			/*!< Mostra um aviso e volta para a tela principal com OK */
		FILES,			/*!< Permite escolha de que arquivo ou diretório mandar */
		TRANSFERING,	/*!< Esperando envio ou recebimento do arquivo */
		FRIEND,			/*!< Mostra mais detalhes sobre um amigo */
		SLIDESHOW,		/*!< Mostra, uma por uma, as imagens do vetor files. */
	};

	///Instância única de P2PTest.
	static P2PTest* _instance;

	/// Componente auxiliar para realização de logs.
	HLoggerPtr logger;

	/// Guarda instância do P2PManager para não precisar ficar acessando toda
	/// vez.
	P2PManager* p2p;
	
	/// Indica a tela atualmente visível da aplição.
	Screen visibleScreen;

	/// Indica o estado da conexção P2P.
	buzz::XmppEngine::State state;

	/// Indica a cor de fundo da interface gráfica.
	IColor* backgroundColor;

	/// Indica a cor de fundo do item selecionado de uma lista da interface
	/// gráfica.
	IColor* selectedItemColor;

	/// Indica a cor da fonte usada na interface gráfica.
	IColor* fontColor;

	/// Indica a cor da fonte do item selecionado de uma lista da interface
	/// gráfica.
	IColor* selectedItemFontColor;

	/// Cor usada para desenhar o botão vermelho na interface gráfica.
	IColor* red;

	/// Cor usada para desenhar o botão verde na interface gráfica.
	IColor* green;

	/// Cor usada para desenhar o botão amarelo na interface gráfica.
	IColor* yellow;

	/// Cor usada para desenhar o botão azul na interface gráfica.
	IColor* blue;

	/// Objeto responsável por gerenciar a Window e a Surface usada pela
	/// aplicação, assim como a fonte a a cor usada.
	/// Não adianta declarar várias GraphicInterfaces, prepará-las e ficar
	/// alternando entre elas, pois mesmo cada uma possuindo uma instância de
	/// surface e de window, existirá interferência entre elas.
	GraphicInterface* gi;

	/// Guarda o valor do Jid para quem o arquivo será enviado. Enquanto estiver
	/// "" significa que não está havendo um envio de arquivo.
	string destinationJid;

	/// Guarda o valor do Status para quem o arquivo será enviado. Enquanto
	/// estiver NULL significa que não está havendo um envio de arquivo.
	buzz::Status* destinationStatus;

	/// Guarda a lista de arquivos da pasta corrente, apresentados para que o
	/// usuário escolha qual deseja enviar, para que posteriormente seja
	/// possível identificar qual o usuário selecionou.
	/// Usado também para guardar lista de imagens contidas na pasta recebida.
	vector<string>* files;

	/// Guarda a lista dos nomes dos amigos conectados.
	vector<string>* friendsName;

	/// Guarda uma lista associada à do nome dos amigos, indicando se ele possui
	/// capacidade de compartilhar arquivos.
	vector<bool>* friendsIsFileShareCapable;

	/// Permite mostrar as imagens recebidas como um slideshow.
	vector<string>::iterator filesIterator;

	GraphicList* gl;

	/// Guarda o nome do arquivo que está sendo recebido ou enviado para
	/// conseguir mostrar seu nome na mensagem de sucesso e conseguir exibir,
	/// caso seja uma imagem recebida.
	string fileTransfered;

	/// Mantém apenas as últimas mensagens visíveis.
	/// Uso list em vez de queue para ter iterator e clear.
	list<string>* chatMessages;

	/**
	 * Constrói um objeto P2PTest.
	 *
	 */
	P2PTest();

	/**
	 * Inicializa a janela que é usada para desenhar na tela.
	 */
	void initializeWindow();

	/**
	 * Mostra a tela de conectado ou de disconectado, dependento do estado
	 * atual da aplicação.
	 */
	void showMainScreen();

	/**
	 * Mostra a tela de disconectado.
	 */
	void showDisconnected();

	/**
	 * Mostra a tela de conectado.
	 */
	void showConnected();

	/**
	 * Mostra a tela de amigo, que contém o chat e permite envio de arquivos.
	 *
	 * @param filename Nome do arquivo de imagem que deve ser mostrado por trás
	 * das mensagens de texto da conversa. Só é diferente de NULL quando o
	 * usuário recebe um evento multimodal contendo uma imagem.
	 */
	void showFriend(string* filename = NULL);

	/**
	 * Mostra uma lista de arquivos para que o usuário escolha qual deseja
	 * enviar.
	 */
	void showPossibleFilesToSend();

	/**
	 * Mostrada quando tenta-se enviar aquivos para alguém que não suporta tal
	 * funcionalidade.
	 */
	void showFileshareCapabilityWarning();

	/**
	 * Mostra mensagem de aguarde e informações sobre o que está sendo enviado
	 * ou recebido.
	 */
	void showTransfering();

	/**
	 * Mostra mensagem de sucesso da transferência de um item.
	 */
	void showSuccess();

	/**
	 * Mostra as imagens, uma por vez, recebidas de um amigo.
	 */
	void showImage();

	/**
	 * Mostra a tela que permite escolha do status atual.
	 */
	void showStatusChoice();

	/**
	 * Mostra a tela de ajuda da aplicação.
	 */
	void showHelp();

	/**
	 * Pergunta ao usuário, via interface gŕafica, se ele aceita receber o
	 * arquivo oferecido a ele.
	 *
	 * @param message Mensagem que deve ser mostrada na interface gráfica.
	 * @return true para aceitar, false para rejeitar.
	 */
	bool showOffer(string message);

	/**
	 * Preenche o atributo files (vetor de string) com o nome das imagens
	 * contidas na pasta passada como parâmetro.
	 *
	 * @param folder Nome da pasta onde serão buscadas imagens.
	 */
	void fillVectorWithImageNames(string folder);

	/**
	 * Preenche o atributo files (vetor de string) com o nome dos amigos
	 * conectados.
	 */
	void fillVectorWithNamesOfFriends();

	/**
	 * Preenche o atributo files (vetor de string) com o nome dos arquivos
	 * contidas na pasta passada como parâmetro.
	 *
	 * @param folder Nome da pasta onde serão buscados arquivos.
	 */
	void fillVectorWithFileNames(string folder);

	/**
	 * Testa se um determinado caminho aponta para uma imagem. Deteca apenas
	 * imagens Gif, JPG, PNG e BMP, através da terminação.
	 *
	 * @param path Caminho a ser testado.
	 */
	bool isImage(string path);

	/**
	 * Testa se um determinado caminho aponta para uma pasta.
	 *
	 * @param path Caminho a ser testado.
	 */
	bool isFolder(string path);

	/**
	 * Lê as informações necessárias para conexão do arquivo de configuração e
	 * solicita conexão.
	 */
	void connect();

	/**
	 * Notifica oferta para receber um arquivo ou pasta ou alguma mudança de
	 * estado relacionada a uma transferência de arquivo ou pasta.
	 *
	 * @param state Indica o que ocorreu.
	 * @param manifest Descrição dos itens que estão sendo oferecidos. Só é
	 * enviado quando o state é OFFER.
	 */
	void transferStateChanged(FileShareClient::State state,
			const cricket::FileShareManifest* manifest);

	/**
	 * Notifica mudança de estado da conexão do P2PManager.
	 *
	 * @param state Indica o novo estado da conexão.
	 */
	void stateChanged(buzz::XmppEngine::State state);

	/**
	 * Notifica mudança de status de algum amigo.
	 *
	 * @param status Indica novo status de um amigo.
	 */
	void friendsStatusChanged(const buzz::Status &status);

	/**
	 * Notifica recebimento de mensagens de texto.
	 *
	 * @param from Bare jid do remetente em forma de string.
	 * @param text Mensagem enviada.
	 */
	void chatMessageReceived(const string& from, const string& text);

	/**
	 * Registra-se como observador de eventos de tecla e multimodais.
	 *
	 */
	void registerListener();

	/**
	 * Transforma um Status em uma string para ser exibida na tela.
	 *
	 * @param statusShow Código correspondente ao status da pessoa.
	 * @return O status em forma de string.
	 */
	string statusToString(buzz::Status::Show statusShow);

	/**
	 * Atualiza a lista de amigos capazes de compartilhar arquivos.
	 */
	void updateFileshareCapableFriends();


public:
	/**
	 * Acessa a instância única do objeto P2PTest.
	 *
	 * @return Instância de P2PTest.
	 */
	static P2PTest* getInstance();

	/**
	 * Destói o objeto P2PTest.
	 */
	~P2PTest();

	/**
	 * Trata toda a interação através do pressionamento de teclas.
	 *
	 * @parm ev Evento a ser tratado.
	 */
	bool userEventReceived(IInputEvent* ev);

	/**
	 * Trata toda a interação multimodal.
	 *
	 * @parm ev Evento a ser tratado.
	 */
	bool multimodalUserEventReceived(IMultimodalInputEvent* ev);
};

}
}
}
}
}
}

#endif //P2PTEST_H
