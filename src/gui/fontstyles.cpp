/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fontstyles.h"

const QString FontStyles::regular("Roboto Regular");
const QString FontStyles::bold("Roboto Bold");
const QString FontStyles::italic("Roboto Italic");
const QString FontStyles::light("Roboto Light");
const QString FontStyles::medium("Roboto Medium");

QFont FontStyles::font(const QString& name, int pixelSize, qreal letterSpacing)
{
    QFont font(name);
    font.setPixelSize(pixelSize);
    font.setLetterSpacing(QFont::AbsoluteSpacing, letterSpacing);
    return font;
}

QFont FontStyles::h4() { return font(regular, 34, 0.25); }
QFont FontStyles::h5() { return font(regular, 24, 0.0); }
QFont FontStyles::h6() { return font(medium, 20, 0.15); }
QFont FontStyles::subtitle1() { return font(regular, 16, 0.15); }
QFont FontStyles::subtitle2() { return font(medium, 14, 0.15); }
QFont FontStyles::body1() { return font(regular, 16, 0.5); }
QFont FontStyles::body2() { return font(regular, 14, 0.25); }
QFont FontStyles::caption() { return font(regular, 12, 0.4); }
