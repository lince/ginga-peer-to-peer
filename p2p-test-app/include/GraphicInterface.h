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
 * @file GraphicInterface.h
 * @author Diogo de Carvalho Pedrosa
 * @date 30-07-10
 */

#ifndef GRAPHICINTERFACE_H
#define GRAPHICINTERFACE_H

#include <linceutil/LoggerUtil.h>
using namespace ::br::ufscar::lince::util;

#include "system/io/interface/output/ISurface.h"
#include "system/io/interface/output/IWindow.h"
#include "system/io/interface/content/text/IFontProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#include "../../telemidia-util-cpp/include/IColor.h"
using namespace ::br::pucrio::telemidia::util;

namespace br {
namespace usp {
namespace icmc {
namespace intermidia {
namespace ginga {
namespace p2ptest {

/**
 * Classe responsável por mostrar texto, imagem e formas geométricas na tela.
 */
class GraphicInterface {
protected:
	/// Componente auxiliar para realização de logs.
	HLoggerPtr logger;

	IWindow* win;
	ISurface* surface;
	IColor* color;
	IColor* bgColor;
	IFontProvider* font;

	/**
	 * Inicializa a janela que é usada para desenhar na tela.
	 */
	void initializeWindow();

public:
	/**
	 * Constrói um objeto GraphicInterface.
	 *
	 */
	GraphicInterface(IColor* color);

	/**
	 * Destói o objeto GraphicInterface.
	 */
	~GraphicInterface();

	void drawString(int x, int y, const char* text);
	void fillRectangle(int x, int y, int width, int height);
	void drawLine(int x1, int y1, int x2, int y2);
	void drawImage(int x, int y, string path);

	void show();
	void clear();

	bool setColor(IColor* color);
	bool setFont(string someUri, int fontSize);

};

}
}
}
}
}
}

#endif // GRAPHICINTERFACE_H
