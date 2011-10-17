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
 * @file GraphicList.h
 * @author Diogo de Carvalho Pedrosa
 * @author José Augusto Costa Martins Júnior
 * @date 30-07-10
 */

#ifndef GRAPHICLIST_H
#define GRAPHICLIST_H

#include <linceutil/LoggerUtil.h>
using namespace ::br::ufscar::lince::util;

#include "../../telemidia-util-cpp/include/IColor.h"
using namespace ::br::pucrio::telemidia::util;


#include <vector>
using namespace std;

#include "GraphicInterface.h"


namespace br {
namespace usp {
namespace icmc {
namespace intermidia {
namespace ginga {
namespace p2ptest {

/**
 * Classe responsável por mostrar texto, imagem e formas geométricas na tela.
 */
class GraphicList {
protected:
	/// Componente auxiliar para realização de logs.
	HLoggerPtr logger;

	/// Objeto usado para mostrar as coisas na tela.
	GraphicInterface* gi;

	/// Lista de strings a serem mostradas.
	vector<string>* itens;

	/// Lista de booleanos que indicam se a imagem deve ser mostrada ou não para
	/// um determinado item.
	vector<bool>* showImage;

	/// Posição x do canto esquerdo.
	int x;

	/// Posição y de cima.
	int y;

	/// Largura do componente.
	int width;

	/// Altura em pixels de cada linha.
	int lineHeight;

	/// Quantidade de linhas que podem ser vistas ao mesmo tempo.
	int numberOfVisibleLines;

	/// Altura do componente. Calculado a partir dos dois atributos anteriores.
	int height;

	/// Cor de fundo desse componente.
	IColor* backgroundColor;

	/// Cor da linha do item selecionado.
	IColor* selectedItemColor;

	/// Cor da fonte do item não selecionado.
	IColor* fontColor;

	/// Cor da fonte do item selecionado.
	IColor* selectedItemFontColor;

	/// Indica qual item da lista está com foco. Começa em 0.
	int focusedItem;

	/// Indica qual item está na primeira posição da lista visível. Feito para
	/// tratar rolagem, ou seja, quando existem mais itens que a área disponível
	/// pode mostrar de uma só vez. Começa em 0.
	int firstItemInPage;

	/// Caminho da imagem que deve ser desenhada antes do texto de cada item.
	string imagePath;

	/// Margem usada entre o limite do componente, os itens e a barra de
	/// rolagem.
	int margin;

	/// Largura do scroll.
	int scrollWidth;

	/// Largura da possível imagem do lado de cada item.
	int imageWidth;


public:
	/**
	 * Constrói um objeto GraphicList. Não deixamos passar a altura do
	 * componente para não deixar acontecer erro de arredondamento.
	 *
	 */
	GraphicList(GraphicInterface* gi1, int x1, int y1, int width1,
			int lineHeight1, int numberOfLines1, IColor* backgroundColor1,
			IColor* selectedItemColor1, IColor* fontColor1,
			IColor* selectedItemFontColor1);

	/**
	 * Destói o objeto GraphicList.
	 */
	~GraphicList();

	/**
	 * Define novamente o primeiro item como o selecionado.
	 */
	void reset();

	/**
	 * Seta uma nova lista de nomes.
	 *
	 * @param itens Lista de strings a serem mostradas.
	 */
	void setItens(vector<string>* itens);

	/**
	 * Seta uma nova lista de nomes. itens deve ter a mesma quantidade de
	 * elemetos que showImage.
	 *
	 * @param itens Lista de strings a serem mostradas.
	 * @param showImage Lista de booleanos que indicam se a imagem deve ser
	 * mostrada ou não para um determinado item.
	 * @param path Caminho da imagem que será usada.
	 */
	void setItens(vector<string>* itens, vector<bool>* showImage,
			string imagePath);

	/**
	 * Atualiza o estado para refletir o pressionamento da tecla seta para
	 * baixo.
	 *
	 * @return true caso alguma mudança de estado ocorra e false caso contrário.
	 */
	bool downPressed();

	/**
	 * Atualiza o estado para refletir o pressionamento da tecla cima para
	 * baixo.
	 *
	 * @return true caso alguma mudança de estado ocorra e false caso contrário.
	 */
	bool upPressed();

	/**
	 * Acessa o valor apontado pelo ítem selecionado.
	 *
	 * @return o valor do ítem selecionado.
	 */
	string getFocusedItemString();

	/**
	 * Acessa o índice do ítem selecionado.
	 *
	 * @return o índice do ítem selecionado.
	 */
	int getFocusedItem();

	/**
	 * Desenha esse componente na GraphicInterface.
	 */
	void draw();

	/**
	 * Desenha a barra de rolagem.
	 */
	void drawScroll();

};

}
}
}
}
}
}

#endif // GRAPHICLIST_H
