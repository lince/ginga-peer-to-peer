/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

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
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

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
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "../config.h"
#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "../../ncl30/include/layout/DeviceLayout.h"
#include "../../gingacc-system/include/io/LocalDeviceManager.h"
#include "../../gingacc-multidevice/include/services/DeviceDomain.h"
#include "../../gingancl/include/multidevice/FormatterMultiDevice.h"
#include "../../gingancl/include/multidevice/FormatterPassiveDevice.h"
#include "../../gingancl/include/multidevice/FormatterActiveDevice.h"
#include "../../gingalssm/include/CommonCoreManager.h"
#include "../../gingalssm/include/PresentationEngineManager.h"
// Para permitir que a main fique travada esperando uma unlock condition
#include "../../gingacc-eim/include/EnhancedInputManager.h"
#endif

#include "ncl/layout/IDeviceLayout.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "system/io/ILocalDeviceManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#include "multidevice/services/IDeviceDomain.h"
using namespace ::br::pucrio::telemidia::ginga::core::multidevice;

#include "gingancl/multidevice/IFormatterMultiDevice.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::multidevice;

#include "gingalssm/ICommonCoreManager.h"
#include "gingalssm/IPresentationEngineManager.h"
using namespace ::br::pucrio::telemidia::ginga::lssm;

#include "mmtestapp/MultimodalTest.h"
using namespace ::br::usp::intermidia::ginga::multimodaltest;

#include "p2ptestapp/P2PTest.h"
using namespace ::br::usp::icmc::intermidia::ginga::p2ptest;

// Para permitir que a main fique travada esperando uma unlock condition
#include "mmi/IEnhancedInputManager.h"
using namespace ::br::ufscar::lince::ginga::core::mmi;


#include "../config.h"

#include <string>
#include <iostream>
using namespace std;

void printHelp() {
	cout << endl << "Usage: gingaNcl [OPTIONS]... [<--ncl> NCLFILE]" << endl;
	cout << "Example: gingaNcl --ncl test.ncl" << endl;
	cout << endl << "OPTIONS are:" << endl;
	cout << "-h, --help                    Display this information." << endl;
	cout << "-i, --insert-delay <value>    Insert a delay before application ";
	cout << "processing." << endl;
	cout << "-i, --insert-oc-delay <value> Insert a delay before tune main A/V ";
	cout << "(to exclusively process OC elementary streams)." << endl;
	cout << "-v, --version                 Display version." << endl;
	cout << "    --enable-log [mode]       Enable log mode:" << endl;
	cout << "                                Turn on verbosity, gather all";
	cout << " output" << endl;
	cout << "                                messages and write them into a";
	cout << " device" << endl;
	cout << "                                according to mode." << endl;
	cout << "                                  [mode] can be:" << endl;
	cout << "                                     'file'   write messages into";
	cout << " a file" << endl;
	cout << "                                              named logFile.txt.";
	cout << endl;
	cout << "                                     'stdout' write messages into";
	cout << " /dev/stdout" << endl;
	cout << "                                     'null'   write messages into";
	cout << " /dev/null" << endl;
	cout << "                                              (default mode).";
	cout << endl;
	cout << "    --x-offset <value>        Offset of left display coord." << endl;
	cout << "    --y-offset <value>        Offset of top display coord." << endl;
	cout << "    --set-width <value>       Force a width display value." << endl;
	cout << "    --set-height <value>      Force a height display value.";
	cout << endl;
	cout << "    --device-class <value>    Define the device class through the";
	cout << endl;
	cout << "                                    presentation" << endl;
	cout << "    --disable-gfx             Disable gfx loading presented during";
	cout << " application processing." << endl;
	cout << "    --enable-automount        Enable trigger for main.ncl ";
	cout << " applications received via a transport protocol." << endl;
	cout << "    --enable-remove-oc-filter Enable processor economies after";
	cout << " first time that the object carousel is mounted." << endl;
	cout << "    --enable-multimodal-test-app Enable the execution of the";
	cout << " application that tests the gingacc-mmi component." << endl;
	cout << "    --enable-p2p-test-app Enable the execution of the application";
	cout << " that tests the gingacc-p2p component." << endl;
}

void printVersion() {
	cout << endl;
	cout << "This is ginga (open source version):  a free " << endl;
	cout << " support for iTV middleware named Ginga-NCL." << endl;
	cout << "Ginga binary v0.11.2." << endl;
	cout << "(c) 2002-2010 The TeleMidia PUC-Rio Team." << endl << endl;
}

string updateFileUri(string file) {
	if (!isAbsolutePath(file)) {
		return getCurrentPath() + file;
	}

	return file;
}

int main(int argc, char *argv[]) {
	ICommonCoreManager* ccm = NULL;
	IPresentationEngineManager* pem = NULL;
	ILocalDeviceManager* dm = NULL;
	IFormatterMultiDevice* fmd = NULL;
#if HAVE_COMPSUPPORT
	IComponentManager* cm;
	cout << "HAVE_COMPSUPPORT" << endl;
#endif

	string nclFile       = "", param = "", bgUri = "", cmdFile = "";
	int i, devClass      = 0;
	int xOffset          = 0, yOffset = 0, w = 0, h = 0, maxTransp = 0;
	double delayTime     = 0;
	double ocDelay       = 0;
	bool isRemoteDoc     = false;
	bool removeOCFilter  = false;
	bool forceQuit       = true;
	bool enableGfx       = true;
	bool autoMount       = false;

	bool enableMultimodalTestApp = false;
	bool enableP2PTestApp = false;

	setLogToNullDev();

	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
			printHelp();
			return 0;

		} else if ((strcmp(argv[i], "-v") == 0) ||
			    (strcmp(argv[i], "--version")) == 0) {

			printVersion();
			return 0;

		} else if ((strcmp(argv[i], "--ncl") == 0) && ((i + 1) < argc)) {
			nclFile.assign(argv[i+1], strlen(argv[i+1]));
			cout << "argv = '" << argv[i+1] << "' nclFile = '";
			cout << nclFile << "'" << endl;

		} else if ((strcmp(argv[i], "--enable-log") == 0) && ((i + 1) < argc)) {
			if (strcmp(argv[i+1], "file") == 0) {
				setLogToFile();

			} else if (strcmp(argv[i+1], "stdout") == 0) {
				setLogToStdoutDev();
			}

		} else if ((strcmp(argv[i], "--force-quit") == 0) && ((i + 1) < argc)) {
			if (strcmp(argv[i+1], "false") == 0) {
				forceQuit = false;
			}

		} else if ((strcmp(argv[i], "--x-offset") == 0) && ((i + 1) < argc)) {
			if (isNumeric(argv[i+1])) {
				param = argv[i+1];
				xOffset = (int)stof(param);
			}

		} else if ((strcmp(argv[i], "--y-offset") == 0) && ((i + 1) < argc)) {
			if (isNumeric(argv[i+1])) {
				param = argv[i+1];
				yOffset = (int)stof(param);
			}

		} else if ((strcmp(argv[i], "--set-width") == 0) && ((i + 1) < argc)) {
			if (isNumeric(argv[i+1])) {
				param = argv[i+1];
				w = (int)stof(param);
			}

		} else if ((strcmp(argv[i], "--set-height") == 0) && ((i + 1) < argc)) {
			if (isNumeric(argv[i+1])) {
				param = argv[i+1];
				h = (int)stof(param);
			}

		} else if ((strcmp(argv[i], "--set-max-transp") == 0) &&
				((i + 1) < argc)) {

			if (isNumeric(argv[i+1])) {
				param = argv[i+1];
				maxTransp = (int)stof(param);
			}

		} else if ((strcmp(argv[i], "--set-bg-image") == 0) &&
				((i + 1) < argc)) {

			param = argv[i+1];
			if (fileExists(param)) {
				bgUri = param;
				cout << "main bgUri = '" << bgUri << "'";
				cout << " OK" << endl;

			} else {
				cout << "main Warning: can't set bgUri '" << param << "'";
				cout << " file does not exist" << endl;
			}

		} else if (((strcmp(argv[i], "--device-class") == 0) ||
				(strcmp(argv[i], "--dev-class") == 0)) && ((i + 1) < argc)) {

			if (strcmp(argv[i+1], "1") == 0 ||
					strcmp(argv[i+1], "passive") == 0) {

				devClass = 1;

			} else if (strcmp(argv[i+1], "2") == 0 ||
					strcmp(argv[i+1], "active") == 0) {

				devClass = 2;
			}

		} else if (((strcmp(argv[i], "-i") == 0) ||
				(strcmp(argv[i], "--insert-delay") == 0)) &&
				((i + 1) < argc)) {

			if (isNumeric(argv[i+1])) {
				param     = argv[i+1];
				delayTime = stof(param);
			}

		} else if (strcmp(argv[i], "--insert-oc-delay") == 0 &&
				((i + 1) < argc)) {

			if (isNumeric(argv[i+1])) {
				param   = argv[i+1];
				ocDelay = stof(param);
			}

		} else if (strcmp(argv[i], "--disable-gfx") == 0) {
			enableGfx = false;

		} else if (strcmp(argv[i], "--enable-automount") == 0) {
			autoMount = true;

		} else if (strcmp(argv[i], "--enable-remove-oc-filter") == 0) {
			removeOCFilter = true;

		} else if ((strcmp(argv[i], "--enable-cmdfile") == 0) &&
				((i + 1) < argc)) {

			cmdFile.assign(argv[i+1], strlen(argv[i+1]));
			cout << "argv = '" << argv[i+1] << "' cmdFile = '";
			cout << cmdFile << "'" << endl;
		} else if (strcmp(argv[i], "--enable-multimodal-test-app") == 0) {
			enableMultimodalTestApp = true;
		} else if (strcmp(argv[i], "--enable-p2p-test-app") == 0) {
			enableP2PTestApp = true;
		}
	}

	if (delayTime > 0) {
		::usleep((int)delayTime);
	}

	initTimeStamp();
	initializeCurrentPath();
#if HAVE_COMPSUPPORT
	cm = IComponentManager::getCMInstance();
#endif

	if (nclFile != "") {
		nclFile = updateFileUri(nclFile);

		if (argc > 1 && nclFile.substr(0, 1) != "/") {
			cout << "ginga main() remote NCLFILE" << endl;
			isRemoteDoc = true;
		}
	}

	cout << "ginga main() NCLFILE = " << nclFile.c_str() << endl;

	if (devClass == 1) {
#if HAVE_COMPSUPPORT
		dm = ((LocalDeviceManagerCreator*)(cm->getObject(
				"LocalDeviceManager")))();

		dm->createDevice("systemScreen(0)");
		fmd = ((FormatterMultiDeviceCreator*)(cm->getObject(
				"FormatterMultiDevice")))(
						NULL, devClass, xOffset, yOffset, w, h);
#else
		dm  = LocalDeviceManager::getInstance();
		dm->createDevice("systemScreen(0)");
		fmd = new FormatterPassiveDevice(NULL, xOffset, yOffset, w, h);
#endif

		if (bgUri != "") {
			fmd->setBackgroundImage(bgUri);
		}
		getchar();

	} else if (devClass == 2) {
#if HAVE_COMPSUPPORT
		dm = ((LocalDeviceManagerCreator*)(cm->getObject(
				"LocalDeviceManager")))();

		dm->createDevice("systemScreen(0)");
		fmd = ((FormatterMultiDeviceCreator*)(cm->getObject(
				"FormatterMultiDevice")))(
						NULL, devClass, xOffset, yOffset, w, h);
#else
		dm  = LocalDeviceManager::getInstance();
		dm->createDevice("systemScreen(0)");
		fmd = new FormatterActiveDevice(NULL, xOffset, yOffset, w, h);
#endif

		if (bgUri != "") {
			fmd->setBackgroundImage(bgUri);
		}

		getchar();

	} else {
		if (nclFile == "") {
			enableGfx = false;
		}

#if HAVE_COMPSUPPORT
		pem = ((PEMCreator*)(cm->getObject("PresentationEngineManager")))(
				devClass, xOffset, yOffset, w, h, enableGfx);
#else
		pem = new PresentationEngineManager(
				devClass, xOffset, yOffset, w, h, enableGfx);
#endif

		if (pem == NULL) {
			return -2;
		}

		if (bgUri != "") {
			cout << endl << endl;
			cout << "main '" << bgUri << "'" << endl;
			cout << endl << endl;
			pem->setBackgroundImage(bgUri);
		}

		if (cmdFile != "") {
			cmdFile = updateFileUri(cmdFile);
			pem->setCmdFile(cmdFile);
		}

		// Enqunto não existe um application manager, inicializamos as
		// aplicações por aqui.

		if (enableP2PTestApp) {
			cout << "Iniciando P2PTestApp" << endl;
			P2PTest *p2pTest = P2PTest::getInstance();
		}

		if (enableMultimodalTestApp) {
			// TODO descobrir por que esse o acesso a instância do InputManager tem
			// que ficar depois do acesso ao PresentationEngineManager.
			cout << "Iniciando MultimodaTestApp" << endl;

			// Acrescenta uma aplicação como observadora de eventos multimodais
			MultimodalTest *mmt = MultimodalTest::getInstance();
			mmt->registerListener();
		}

		// Pode-se postar um evento de 3 formas diferentes:
		// 1) Lendo o conteúdo do arquivo e passando a string resultante.
		// 2) Passando o nome do arquivo
		// 3) Montando uma string da hora e passando a string montada.
		// Nenhuma das 3 está sendo usada aqui pois o xml está sendo recebido
		// pelo CommunicationManager.

		// Forma 1)
//		string str, strTotal;
//		ifstream in;
//		in.open("/misc/eventoMultimodal.xml");
//		getline(in, str);
//
//		while (in) {
//		   strTotal += str + '\n';
//		   getline(in, str);
//		}
//
//		im->postMultimodalEvent(strTotal);

		// Forma 2)
//		im->postMultimodalEvent("/misc/eventoMultimodal.xml");

		// Forma 3)
//		im->postMultimodalEvent(
//				"<multimodal id=\"idDoEventoDeTeste\">\n"
//				"    <head>\n"
//				"        <device id=\"a\" model=\"z\"/>\n"
//				"        <user id=\"q\"/>\n"
//				"        <time begin=\"dateTime\" end=\"dateTime\"/>\n"
//				"    </head>\n"
//				"    <body>\n"
//				"        <inkml:ink>\n"
//				"            <inkml:trace>\n"
//				"                10 0, 9 14, 8 28, 7 42, 6 56, 6 70, 8 84, 8 98, 8 112, 9 126, 10 140,\n"
//				"                13 154, 14 168, 17 182, 18 188, 23 174, 30 160, 38 147, 49 135,\n"
//				"                58 124, 72 121, 77 135, 80 149, 82 163, 84 177, 87 191, 93 205\n"
//				"            </inkml:trace>\n"
//				"            <inkml:trace>\n"
//				"                130 155, 144 159, 158 160, 170 154, 179 143, 179 129, 166 125,\n"
//				"                152 128, 140 136, 131 149, 126 163, 124 177, 128 190, 137 200,\n"
//				"                150 208, 163 210, 178 208, 192 201, 205 192, 214 180\n"
//				"            </inkml:trace>\n"
//				"            <inkml:trace>\n"
//				"                227 50, 226 64, 225 78, 227 92, 228 106, 228 120, 229 134,\n"
//				"                230 148, 234 162, 235 176, 238 190, 241 204\n"
//				"            </inkml:trace>\n"
//				"            <inkml:trace>\n"
//				"                282 45, 281 59, 284 73, 285 87, 287 101, 288 115, 290 129,\n"
//				"                291 143, 294 157, 294 171, 294 185, 296 199, 300 213\n"
//				"            </inkml:trace>\n"
//				"            <inkml:trace>\n"
//				"                366 130, 359 143, 354 157, 349 171, 352 185, 359 197,\n"
//				"                371 204, 385 205, 398 202, 408 191, 413 177, 413 163,\n"
//				"                405 150, 392 143, 378 141, 365 150\n"
//				"            </inkml:trace>\n"
//				"        </inkml:ink>\n"
//				"        <voice>aqui entra um voicexml</voice>\n"
//				"        <accel>Ainda por definir</accel>\n"
//				"        <binary>Ainda por definir</binary>\n"
//				"    </body>\n"
//				"</multimodal>");


		if (nclFile == "") {
			pem->setIsLocalNcl(false, NULL);
			pem->autoMountOC(autoMount);

#if HAVE_COMPSUPPORT
			ccm = ((CCMCreator*)(cm->getObject("CommonCoreManager")))(pem);

#else
			ccm = new CommonCoreManager(pem, xOffset, yOffset, w, h);
#endif
			ccm->removeOCFilterAfterMount(removeOCFilter);
			ccm->setOCDelay(ocDelay);
			ccm->startPresentation();

			// Essa trecho faz com que o fluxo trave até que o método
			// unlockConditionSatisfied do eim seja chamado. Isso permite sair
			// do ginga recebendo a mensagem MAIN ALL DONE e, ao mesmo tempo,
			// permite que o ginga permaneça em execução mesmo se não existir
			// uma aplicação ncl em execução e se não estiver ocorrendo o
			// recebimento de um ts.
			// Permite também, por exemplo, que a aplicação de teste do
			// componente P2P permaneça em execução esperando pelo recebimento
			// de algum arquivo.
			// A chamada ao método unlockConditionSatisfied está sendo feita no
			// método EnhancedInputManager::run(), quando a tecla F10 é
			// pressionada.
			IEnhancedInputManager* eim;
#if HAVE_COMPSUPPORT
			eim = ((EnhancedInputManagerCreator*)
				(cm->getObject("InputManager")))();
#else
			eim = EnhancedInputManager::getInstance();
#endif
			cout << "Vou esperar liberação do EnhancedInputManager" << endl;
			eim->waitForUnlockCondition();

		} else if (fileExists(nclFile) || isRemoteDoc) {
			cout << "ginga main() NCLFILE = " << nclFile.c_str() << endl;
			pem->setIsLocalNcl(forceQuit, NULL);
			if (pem->openNclFile(nclFile)) {
				pem->startPresentation(nclFile);
				pem->waitUnlockCondition();
			}
		}

		if (pem != NULL) {
			delete pem;
		}

		if (ccm != NULL) {
			delete ccm;
		}

#if HAVE_COMPSUPPORT
		printTimeStamp();
		cm->releaseComponentFromObject("PresentationEngineManager");
#endif
	}

	cout << "MAIN ALL DONE!" << endl;
	if (!forceQuit) {
		cout << "Press a key to continue" << endl;
		getchar();
	}

	return 0;
}
