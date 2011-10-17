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
 * @file GraphicList.cpp
 * @author Diogo de Carvalho Pedrosa
 * @author José Augusto Costa Martins Júnior
 * @date 30-07-10
 */

#include "../include/GraphicList.h"

namespace br {
namespace usp {
namespace icmc {
namespace intermidia {
namespace ginga {
namespace p2ptest {

GraphicList::GraphicList(GraphicInterface* gi1, int x1, int y1, int width1,
		int lineHeight1, int numberOfVisibleLines1, IColor* backgroundColor1,
		IColor* selectedItemColor1, IColor* fontColor1,
		IColor* selectedItemFontColor1) :
		gi(gi1),
		x(x1),
		y(y1),
		width(width1),
		lineHeight(lineHeight1),
		numberOfVisibleLines(numberOfVisibleLines1),
		height(lineHeight * numberOfVisibleLines),
		backgroundColor(backgroundColor1),
		selectedItemColor(selectedItemColor1),
		fontColor(fontColor1),
		selectedItemFontColor(selectedItemFontColor1),
		firstItemInPage(0),
		focusedItem(0),
		itens(NULL),
		showImage(NULL),
		margin(5),
		scrollWidth(21),
		imageWidth(30) {
	logger = LoggerUtil::getLogger("ginga.p2ptest.GraphicList");
	LoggerUtil::configure();
	LoggerUtil_info(logger, "Construtor");


	LoggerUtil_info(logger, "Fim do construtor");
}

GraphicList::~GraphicList() {
	LoggerUtil_info(logger, "Destrutor");
}

void GraphicList::reset() {
	LoggerUtil_info(logger, "reset");
	firstItemInPage = 0;
	focusedItem = 0;
}

void GraphicList::setItens(vector<string>* itens) {
	LoggerUtil_info(logger, "setItens(vector<string>* itens)");
	this->itens = itens;

	imagePath = "";

	if (focusedItem > itens->size() - 1) {
		focusedItem = itens->size() - 1;

		if (focusedItem - (numberOfVisibleLines - 1) < 0) {
			firstItemInPage = 0;
		} else {
			firstItemInPage = focusedItem - (numberOfVisibleLines - 1);
		}
	}
}

void GraphicList::setItens(vector<string>* itens, vector<bool>* showImage,
		string imagePath) {
	LoggerUtil_info(logger, "setItens(vector<string>* itens, "
			"vector<bool>* showImage, string imagePath)");
	if (itens->size() == showImage->size()) {
		setItens(itens);
		this->showImage = showImage;
		this->imagePath = imagePath;
	} else {
		// TODO Lançar excessão.
		LoggerUtil_info(logger, "O vetor showImage deve ter o mesmo tamanho do "
				"vetor itens.");
	}
}

bool GraphicList::upPressed() {
	LoggerUtil_info(logger, "upPressed");
	if (focusedItem > 0) {
		if (focusedItem == firstItemInPage) {
			firstItemInPage--;
		}
		focusedItem--;
		return true;
	}

	return false;
}

bool GraphicList::downPressed() {
	LoggerUtil_info(logger, "downPressed");
	if (focusedItem < itens->size() - 1) {
		focusedItem++;

		if (firstItemInPage == focusedItem - numberOfVisibleLines) {
			firstItemInPage++;
		}

		return true;
	}

	return false;
}

string GraphicList::getFocusedItemString() {
	LoggerUtil_info(logger, "getFocusedItemString");
	return itens->at(focusedItem);
}

int GraphicList::getFocusedItem() {
	LoggerUtil_info(logger, "getFocusedItem");
	return focusedItem;
}

void GraphicList::draw() {
	LoggerUtil_info(logger, "draw");

	gi->setColor(backgroundColor);
	gi->fillRectangle(x, y, width, height + 2 * margin);
	gi->setColor(fontColor);

	// Testa se precisa de scroll.
	if (itens->size() > numberOfVisibleLines) {
		drawScroll();
	}

	int yTemp = y + margin;

	// Desenha cada um dos itens visíveis.
	for (int i = 0; i < numberOfVisibleLines && i < itens->size(); i++) {
		// Testa se é o item selecionado.
		if (focusedItem - firstItemInPage == i) {
			gi->setColor(selectedItemColor);
			gi->fillRectangle(x + margin,
					yTemp,
					width - scrollWidth - 3 * margin,
					lineHeight);
			gi->setColor(selectedItemFontColor);
		} else {
			gi->setColor(fontColor);
		}

		if (imagePath.compare("") != 0) {
			gi->drawString(x + 3 * margin + imageWidth, yTemp,
					itens->at(firstItemInPage + i).c_str());

			if (showImage->at(firstItemInPage + i)) {
				gi->drawImage(x + 2 * margin, yTemp + margin, imagePath);
			}
		} else {
			gi->drawString(x + 2 * margin, yTemp,
					itens->at(firstItemInPage + i).c_str());
		}

		yTemp += lineHeight;
	}

	// Para quando sair do laço com o último item focado.
	gi->setColor(fontColor);

	LoggerUtil_info(logger, "Fim de draw");
}

void GraphicList::drawScroll() {
	LoggerUtil_info(logger, "showScroll");
	int scrollHeight = height / (itens->size() - numberOfVisibleLines);

	// Limita o tamanho mínimo da barra, para não ficar muito pequena.
	if (scrollHeight < lineHeight) {
		scrollHeight = lineHeight;
	}

	// Para corrigir erros de arredondamento, posiciona na mão a barra em
	// sua última posição.
	if (firstItemInPage == itens->size() - numberOfVisibleLines) {
		gi->fillRectangle(x + width - scrollWidth - margin,
				y + height - scrollHeight + margin,
				scrollWidth,
				scrollHeight);
	} else {
		int yScroll = y + firstItemInPage * ((height - scrollHeight) /
				(itens->size() - numberOfVisibleLines));

		gi->fillRectangle(x + width - scrollWidth - margin,
				yScroll + margin,
				scrollWidth,
				scrollHeight);
	}

	// A linha possui um pixel de largura, por isso a largura da barra é
	// ímpar e a altura da linha é 1 pixel a menos do que a área disponível.
	gi->drawLine(x + width - scrollWidth/2 - margin,
			y + margin,
			x + width - scrollWidth/2 - margin,
			y + height - 1); // 1 = largura da linha
}

}
}
}
}
}
}
