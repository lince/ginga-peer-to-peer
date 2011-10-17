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
Maria da Graça Pimentel - mgp@icmc.usp.br
http://www.ncl.org.br
http://www.ginga.org.br
http://agua.intermidia.icmc.usp.br/intermidia
 *******************************************************************************/

/**
 * @file FileShareClient.cpp
 * @author Armando Biagioni Neto
 * @author Diogo de Carvalho Pedrosa
 * @author José Augusto Costa Martins Júnior
 * @date 22-07-10
 */

#include "../include/FileShareClient.h"
#include "../include/IP2PEventListener.h"


#include <iostream>	// Para cout
#include <iomanip>	// Para setprecision
using namespace std;

#include <sys/ioctl.h>	// Para winsize (barra de progresso)

#include <talk/base/fileutils.h>	// Para talk_base::Filesystem
#include <talk/base/pathutils.h>	// Para talk_base::Pathname (parâmetro das funções de Filesystem)
#include <talk/base/httpclient.h>	// Para HttpTransaction (OnResampleImage)


namespace br {
namespace usp {
namespace icmc {
namespace intermidia {
namespace ginga {
namespace core {
namespace p2p {

FileShareClient::FileShareClient(string root_dir) :
		root_dir_(root_dir),
		send_to_jid_(buzz::JID_EMPTY),
		manifest_(NULL),
		session_(NULL),
		listener(NULL) {

	logger = LoggerUtil::getLogger("ginga.core.p2p.FileShareClient");
    LoggerUtil::configure();
	LoggerUtil_info(logger, "Construtor");
}

FileShareClient::~FileShareClient(){
	LoggerUtil_info(logger, "Destrutor");

	if (manifest_ != NULL) {
		delete manifest_;
		manifest_ = NULL;
	}
}

void FileShareClient::setListener(IP2PEventListener* listener) {
	LoggerUtil_info(logger, "setListener");

	this->listener = listener;
}

void FileShareClient::sendFiles(vector<string>* files,
		const buzz::Jid &send_to, map<string, buzz::Status>* roster) {

	LoggerUtil_info(logger, "setManifest");

	// Constrói um manifesto a partir do vetor recebido.
	cricket::FileShareManifest* tempManifest = new cricket::FileShareManifest();

	for (vector<string>::iterator it = files->begin(); it != files->end();
			it++) {
		cout << "inicio da iteração" << endl;

		// Additionally, we should check for image files here, and call
		// AddImage on the manifest with their file size and image size.
		// The receiving client can then request previews of those images
		if (talk_base::Filesystem::IsFolder(std::string(*it))) {
			const char* dirName = (*it).c_str();
			tempManifest->AddFolder(*it, getDirSize(dirName));
		} else {
			if (talk_base::Filesystem::FileExists(string(*it))) {
				size_t size = 0;
				talk_base::Filesystem::GetFileSize(string(*it), &size);
				cout << "size = " << size << endl;
				tempManifest->AddFile(*it, size);
				cout << "depois de addFile" << size << endl;
			} else {
				LoggerUtil_info(logger, "Arquivo \"" << *it <<
						"\" não existe!");
			}
		}
		cout << "Fim da iteração" << endl;
	}

	// Se o manifesto construido possuir algum item, tenta enviar (o manifesto
	// será deletado automaticamente depois do envio dos itens), caso
	// contrário deleta o manifesto e avisa que nenhum arquivo será enviado.
	if (tempManifest->GetFileCount() > 0 ||
			tempManifest->GetFolderCount() > 0 ||
			tempManifest->GetImageCount() > 0) {
		manifest_ = tempManifest;
		send_to_jid_ = send_to;

		// Se o destinatário já estiver conectado, envia o manifesto.
		map<string, buzz::Status>::iterator iter = roster->begin();

		while (iter != roster->end()) {
			if (iter->second.jid().BareEquals(send_to) &&
					iter->second.fileshare_capability()) {
				LoggerUtil_info(logger, "Achei! O amigo já está conectado.");
				cricket::FileShareSession* share =
						file_share_session_client_->CreateFileShareSession();

				share->Share(iter->second.jid(),
						const_cast<cricket::FileShareManifest*>(manifest_));

				send_to_jid_ = buzz::JID_EMPTY;
				// TODO Permitir envio em paralelo para todas as conexões em que o
				// usuário é capaz de compartilhar arquivos.
				break;
			}

			iter++;
		}
	} else {
		delete tempManifest;
		LoggerUtil_info(logger, "Nenhum arquivo será enviado!");
	}

	LoggerUtil_info(logger, "Fim de setManifest");
}

unsigned int FileShareClient::getDirSize(const char *directory) {
	LoggerUtil_info(logger, "getDirSize(const char *directory)");

	unsigned int total = 0;
	talk_base::DirectoryIterator iter;
	talk_base::Pathname path;
	path.AppendFolder(directory);
	iter.Iterate(path.pathname());

	while (iter.Next())  {
		if (iter.Name() == "." || iter.Name() == "..") {
			continue;
		}
		if (iter.IsDirectory()) {
			path.AppendPathname(iter.Name());
			total += getDirSize(path.pathname().c_str());
		} else {
			total += iter.FileSize();
		}
	}

	return total;
}

void FileShareClient::OnMessage(talk_base::Message *m) {
	LoggerUtil_info(logger, "OnMessage");

//	cout << "CurrentThread() = " << talk_base::ThreadManager::CurrentThread() << endl;

	switch(m->message_id) {
		case MSG_STOP: {
			talk_base::Thread *thread = talk_base::ThreadManager::CurrentThread();
			delete session_;
			session_ = NULL;
			thread->Stop();

			// TODO PENSAR COMO DELETAR A THREAD!
		//	delete thread;
		//	thread = NULL;
			break;
		}
		case MSG_DECLINE: {
			session_->Decline();
			break;
		}
		case MSG_ACCEPT: {
			session_->Accept();
			break;
		}
	}

	LoggerUtil_info(logger, "Fim de OnMessage");
}

string FileShareClient::filesizeToString(unsigned int size) {
	LoggerUtil_info(logger, "filesizeToString");
	double size_display;
	string format;
	stringstream ret;

	// the comparisons to 1000 * (2^(n10)) are intentional
	// it's so you don't see something like "1023 bytes",
	// instead you'll see ".9 KB"

	if (size < 1000) {
		format = "Bytes";
		size_display = size;
	} else if (size < 1000 * 1024) {
		format = "KiB";
		size_display = (double)size / 1024.0;
	} else if (size < 1000 * 1024 * 1024) {
		format = "MiB";
		size_display = (double)size / (1024.0 * 1024.0);
	} else {
		format = "GiB";
		size_display = (double)size / (1024.0 * 1024.0 * 1024.0);
	}

	ret << std::setprecision(1) << std::setiosflags(std::ios::fixed) <<
			size_display << " " << format;

	LoggerUtil_info(logger, "Fim de filesizeToString");
	return ret.str();
}

void FileShareClient::OnSessionState(cricket::FileShareState state) {
	LoggerUtil_info(logger, "OnSessionState");
	talk_base::Thread *thread = talk_base::ThreadManager::CurrentThread();
	std::stringstream manifest_description;

	switch(state) {
	case cricket::FS_OFFER:

		// The offer has been made; print a summary of it and, if it's an
		// incoming transfer, accept it
		// TODO Como fica a descrição se houver imagens???
		if (session_->manifest()->size() == 1) {
			manifest_description <<  session_->manifest()->item(0).name;
		} else if (session_->manifest()->GetFileCount() &&
				session_->manifest()->GetFolderCount()) {
			manifest_description <<  session_->manifest()->GetFileCount() <<
					" arquivos e " << session_->manifest()->GetFolderCount() <<
					" diretórios";
		} else if (session_->manifest()->GetFileCount() > 0) {
			manifest_description <<  session_->manifest()->GetFileCount() <<
					" arquivos";
		} else {
			manifest_description <<  session_->manifest()->GetFolderCount() <<
					" diretórios";
		}

		size_t filesize;

		if (!session_->GetTotalSize(filesize)) {
			manifest_description << " (Tamanho desconhecido)";
		} else {
			manifest_description << " (" << filesizeToString(filesize) << ")";
		}

		if (session_->is_sender()) {
			LoggerUtil_info(logger, "Oferecendo " << manifest_description.str() <<
					" para " << send_to_jid_.Str());
		} else {
			LoggerUtil_info(logger, "Recebendo " << manifest_description.str()
					<< " de " << session_->jid().BareJid().Str());
//			session_->Accept();

			// Avisa ao observador que um arquivo está sendo oferecido.
			if (listener != NULL) {
				listener->transferStateChanged(OFFER, session_->manifest());
			}

			// If this were a graphical client, we might want to go through the
			// manifest, look for images, and request previews. There are two
			// ways to go about this:
			//
			// If we want to display the preview in a web browser (like the
			// embedded IE control in Google Talk), we could call
			// GetImagePreviewUrl on the session, with the image's index in the
			// manifest, the size, and a pointer to the URL.
			// This will cause the session to listen for HTTP requests on
			// localhost, and set url to a localhost URL that any
			// web browser can use to get the image preview:
			//
			//      std::string url;
			//      session_->GetImagePreviewUrl(0, 100, 100, &url);
			//      url = std::string("firefox \"") + url + "\"";
			//      system(url.c_str());
			//
			// Alternately, you could use libjingle's own HTTP code with the
			// FileShareSession's SocketPool interface to
			// write the image preview directly into a StreamInterface:
			//
			//	talk_base::HttpClient *client =
			//      new talk_base::HttpClient("pcp", session_);
			//	std::string path;
			//	session_->GetItemNetworkPath(0,1,&path);
			//
			//	client->request().verb = talk_base::HV_GET;
			//	client->request().path = path + "?width=100&height=100";
			//	talk_base::FileStream *file = new talk_base::FileStream;
			//	file->Open("/home/username/foo.jpg", "wb");
			//	client->response().document.reset(file);
			//	client->start();
		}
		break;
	case cricket::FS_TRANSFER:
		LoggerUtil_info(logger, "File transfer started.");
		// Avisa ao observador que transferência foi iniciada.
		if (listener != NULL) {
			listener->transferStateChanged(TRANSFER, NULL);
		}
		break;
	case cricket::FS_COMPLETE:
		// Comentei essa linha para que seja possível receber arquivos mais de
		// uma vez.
//		thread->Post(this, MSG_STOP);
		LoggerUtil_info(logger, std::endl << "File transfer completed.");

		// O manifesto é deletado automaticamente.
		manifest_ = NULL;

		// Avisa ao observador que transferência foi completada.
		if (listener != NULL) {
			listener->transferStateChanged(COMPLETE, NULL);
		}
		break;
	case cricket::FS_LOCAL_CANCEL:
		LoggerUtil_info(logger, std::endl << "File transfer cancelled locally.");
		// TODO Será que devemos comentar essa linha tb para que seja possível
		// receber arquivos depois de um cancelamento? Preciso fazer uns testes
		// simples.
//		thread->Post(this, MSG_STOP);

		// O manifesto é deletado automaticamente.
		manifest_ = NULL;

		// Avisa ao observador que transferência foi cancelada localmente.
		if (listener != NULL) {
			listener->transferStateChanged(LOCAL_CANCEL, NULL);
		}
		break;
	case cricket::FS_REMOTE_CANCEL:
		LoggerUtil_info(logger, std::endl << "File transfer cancelled remotelly.");
		// TODO Será que devemos comentar essa linha tb para que seja possível
		// receber arquivos depois de um cancelamento? Preciso fazer uns testes
		// simples.
//		thread->Post(this, MSG_STOP);

		// O manifesto é deletado automaticamente.
		manifest_ = NULL;

		// Avisa ao observador que transferência foi cancelada remotamente.
		if (listener != NULL) {
			listener->transferStateChanged(REMOTE_CANCEL, NULL);
		}
		break;
	case cricket::FS_FAILURE:
		LoggerUtil_info(logger, std::endl << "File transfer failed.");
		// TODO Será que devemos comentar essa linha tb para que seja possível
		// receber arquivos depois de uma falha? Como não sei simular o estado
		// de falha, vou deixar comentado mesmo.
//		thread->Post(this, MSG_STOP);

		// Avisa ao observador que houve falha durante transferência.
		if (listener != NULL) {
			listener->transferStateChanged(FAILURE, NULL);
		}
		break;
	}

	LoggerUtil_info(logger, "Fim de OnSessionState");
}

void FileShareClient::OnUpdateProgress(cricket::FileShareSession *sess) {
	// Progress has occured on the transfer; update the UI
	LoggerUtil_info(logger, "OnUpdateProgress");
	size_t totalsize, progress;
	std::string itemname;
	unsigned int width = 79;

	struct winsize ws;
	if ((ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)) {
		width = ws.ws_col;
	}

	if(sess->GetTotalSize(totalsize) && sess->GetProgress(progress) &&
			sess->GetCurrentItemName(&itemname)) {
		float percent = (float)progress / totalsize;
		unsigned int progressbar_width = (width * 4) / 5;

		const char *filename = itemname.c_str();
		std::cout.put('\r');
		for (unsigned int l = 0; l < width; l++) {
			if (l < percent * progressbar_width) {
				std::cout.put('#');
			} else if (l > progressbar_width &&
					l < progressbar_width + 1 + strlen(filename)) {
				std::cout.put(filename[l-(progressbar_width + 1)]);
			} else {
				std::cout.put(' ');
			}
		}
		std::cout.flush();
	}
	LoggerUtil_info(logger, "Fim de OnUpdateProgress");
}

void FileShareClient::OnResampleImage(std::string path, int width, int height,
		talk_base::HttpTransaction *trans) {
	LoggerUtil_info(logger, "OnResampleImagem");

	// The other side has requested an image preview. This is an asynchronous
	// request. We should resize the image to the requested size,and send that
	// to ResampleComplete(). For simplicity, here, we send back the original
	// sized image. Note that because we don't recognize images in our manifest
	// this will never be called in pcp

	// Even if you don't resize images, you should implement this method and
	// connect to the SignalResampleImage signal, just to return an error.

	talk_base::FileStream *s = new talk_base::FileStream();
	if (s->Open(path.c_str(), "rb")) {
		session_->ResampleComplete(s, trans, true);
	} else {
		delete s;
		session_->ResampleComplete(NULL, trans, false);
	}

	LoggerUtil_info(logger, "Fim de OnResampleImagem");
}

void FileShareClient::OnFileShareSessionCreate(
		cricket::FileShareSession *sess) {
	LoggerUtil_info(logger, "OnFileShareSessionCreate");

	session_ = sess;
	sess->SignalState.connect(this, &FileShareClient::OnSessionState);
	sess->SignalNextFile.connect(this, &FileShareClient::OnUpdateProgress);
	sess->SignalUpdateProgress.connect(this,
			&FileShareClient::OnUpdateProgress);
	sess->SignalResampleImage.connect(this, &FileShareClient::OnResampleImage);
	sess->SetLocalFolder(root_dir_);

	LoggerUtil_info(logger, "Fim de OnFileShareSessionCreate");
}

void FileShareClient::onSignon(cricket::SessionManager* session_manager,
		const buzz::Jid jid) {
	LoggerUtil_info(logger, "onSignon");

	// Terceiro parâmetro é o userAgent.
	// O método reset deleta o objeto apontado e coloca um novo objeto no lugar.
	file_share_session_client_.reset(new cricket::FileShareSessionClient(
			session_manager, jid, "Ginga"));
	file_share_session_client_->SignalFileShareSessionCreate.connect(this,
			&FileShareClient::OnFileShareSessionCreate);
	session_manager->AddClient(NS_GOOGLE_SHARE,
			file_share_session_client_.get());

	LoggerUtil_info(logger, "Fim de onSignon");
}

void FileShareClient::onPossibleDestinationOnline(const buzz::Status &status) {
	LoggerUtil_info(logger, "onPossibleDestinationOnline");

	// A contact's status has changed. If the person we're looking for is
	// online and able to receive files, send it.
	if (send_to_jid_.BareEquals(status.jid())) {
		cout << send_to_jid_.Str() << " has signed on." << endl;
		cricket::FileShareSession* share =
				file_share_session_client_->CreateFileShareSession();
		share->Share(status.jid(),
				const_cast<cricket::FileShareManifest*>(manifest_));
		send_to_jid_ = buzz::JID_EMPTY;
	}

	LoggerUtil_info(logger, "Fim de onPossibleDestinationOnline");
}

}
}
}
}
}
}
}

