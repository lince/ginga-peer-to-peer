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
 * @file GraphicInterface.cpp
 * @author Diogo de Carvalho Pedrosa
 * @date 30-07-10
 */

#include "../include/GraphicInterface.h"

//#if HAVE_COMPSUPPORT
//#include "system/io/ILocalDeviceManager.h"
//#include "system/io/interface/content/image/IImageProvider.h"
//#else
#include "../../gingacc-cpp/gingacc-system/include/io/interface/output/dfb/DFBSurface.h"
#include "../../gingacc-cpp/gingacc-system/include/io/interface/output/dfb/DFBWindow.h"
#include "../../gingacc-cpp/gingacc-system/include/io/LocalDeviceManager.h"
#include "../../gingacc-cpp/gingacc-system/include/io/interface/content/text/DFBFontProvider.h"

#include "../../telemidia-util-cpp/include/functions.h"
#include "../../telemidia-util-cpp/include/Color.h"
#include "../../gingacc-cpp/gingacc-system/include/io/interface/content/image/DFBImageProvider.h"
//#endif

//#if HAVE_COMPSUPPORT
//#include "cm/IComponentManager.h"
//using namespace ::br::pucrio::telemidia::ginga::core::cm;
//#else
//#endif


namespace br {
namespace usp {
namespace icmc {
namespace intermidia {
namespace ginga {
namespace p2ptest {

	GraphicInterface::GraphicInterface(IColor* color) {
		logger = LoggerUtil::getLogger("ginga.p2ptest.GraphicInterface");
        LoggerUtil::configure();
		LoggerUtil_info(logger, "Construtor");

		win = NULL;

		// TODO Passar a usar o COMPSURPPORT
		// TODO Pegar tamanho da tela do device
#if HAVE_COMPSUPPORT
		this->surface = ((SurfaceCreator*)(cm->getObject("Surface")))(
				NULL, 720, 576);
#else
		this->surface = new DFBSurface(720, 576);
#endif

		// Acho que funciona sem isso.
//		if (this->surface != NULL) {
//			this->surface->setCaps(this->surface->getCap("ALPHACHANNEL"));
//		}

		setColor(color);
		bgColor = new Color(10, 10, 10);

		clear();

		font = NULL;
		setFont("/usr/local/etc/ginga/files/font/vera.ttf", 32);

		LoggerUtil_info(logger, "Fim do construtor");
	}

	GraphicInterface::~GraphicInterface() {
		delete bgColor;
		// TODO Deletar o resto das coisas.
	}

	void GraphicInterface::initializeWindow() {
		int x = 0, y, w, h;
		ILocalDeviceManager* dm = NULL;
#if HAVE_COMPSUPPORT
		dm = ((LocalDeviceManagerCreator*)(
				cm->getObject("LocalDeviceManager")))();
#else
		dm = LocalDeviceManager::getInstance();
#endif

		if (dm != NULL) {
			x = 0;
			y = 0;
			w = (int)(dm->getDeviceWidth());
			h = (int)(dm->getDeviceHeight());
		}

#if HAVE_COMPSUPPORT
		win = ((WindowCreator*)(cm->getObject("Window")))(x, y, w, h);
#else
		win = new DFBWindow(x, y, w, h);
#endif

		win->setCaps(win->getCap("ALPHACHANNEL"));
		win->draw();
	}

	void GraphicInterface::drawString(int x, int y, const char* text) {
//		LoggerUtil_info(logger, "drawString(string text, int x, int y)");
		this->surface->drawString(x, y, text);
	}

	void GraphicInterface::fillRectangle(int x, int y, int width, int height) {
//		LoggerUtil_info(logger, "drawString(string text, int x, int y)");
		this->surface->fillRectangle(x, y, width, height);
	}

	void GraphicInterface::drawLine(int x1, int y1, int x2, int y2) {
		this->surface->drawLine(x1, y1, x2, y2);
	}

	void GraphicInterface::drawImage(int x, int y, string mrl) {
//		LoggerUtil_info(logger, "drawImage(string path)");

		IImageProvider* provider;

		if (fileExists(mrl)) { // Em telemidia-util-cpp/include/functions.h
#if HAVE_COMPSUPPORT
			provider = ((ImageProviderCreator*)(cm->getObject(
					"ImageProvider")))(mrl.c_str());
#else
			provider = new DFBImageProvider(mrl.c_str());
#endif
		}

		ISurface* imageSurface = provider->prepare(false);
		delete provider;

		int width;
		int height;
		imageSurface->getSize(&width, &height);
		surface->blit(x, y, imageSurface, 0, 0, width, height);
		delete imageSurface;
	}

	void GraphicInterface::show() {
//		LoggerUtil_info(logger, "show");

		if (win == NULL) {
			initializeWindow();
		}

		if (surface->setParent((void*)win)) {
			win->renderFrom(surface);
		}

		win->show();
		win->raiseToTop();
	}

	void GraphicInterface::clear() {
//		LoggerUtil_info(logger, "clear");

		this->surface->clearSurface();

		this->surface->setColor(bgColor);
		this->surface->fillRectangle(0, 0, 720, 576);
		this->surface->setColor(color);
	}

	bool GraphicInterface::setColor(IColor* color) {
//		LoggerUtil_info(logger, "setColor(IColor* color)");

		this->color = color;
		this->surface->setColor(color);

	}

	bool GraphicInterface::setFont(string someUri, int fontSize) {
//		LoggerUtil_info(logger, "setFont " << someUri);

		if (!fileExists(someUri)) {
			LoggerUtil_info(logger, "Erro! Não fonte: '" << someUri <<
					"' não existe");
			return false;
		}

		if (font != NULL) {
			delete font;
			font = NULL;
//#if HAVE_COMPSUPPORT
//			cm->releaseComponentFromObject("FontProvider");
//#endif
		}

//#if HAVE_COMPSUPPORT
//		font = ((FontProviderCreator*)(cm->getObject("FontProvider")))(
//				someUri.c_str(), fontSize);
//#else
		font = new DFBFontProvider(someUri.c_str(), fontSize);
//#endif

		if (font == NULL || font->getContent() == NULL) {
			LoggerUtil_info(logger, "Erro! Não foi possível criar fonte '" <<
					someUri << "' não existe");
			return false;
		}

		surface->setFont(font);

		return true;
	}

}
}
}
}
}
}
