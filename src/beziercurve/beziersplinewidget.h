/***************************************************************************
 *   Copyright (C) 2010 by Till Theato (root@ttill.de)                     *
 *   This file is part of Kdenlive (www.kdenlive.org).                     *
 *                                                                         *
 *   Kdenlive is free software: you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Kdenlive is distributed in the hope that it will be useful,           *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with Kdenlive.  If not, see <http://www.gnu.org/licenses/>.     *
 ***************************************************************************/

#ifndef BEZIERSPLINEWIDGET_H
#define BEZIERSPLINEWIDGET_H

#include "cubicbezierspline.h"
#include "beziersplineeditor.h"
#include "ui_bezierspline_ui.h"

#include <QtCore>
#include <QWidget>

class BezierSplineWidget : public QWidget
{
    Q_OBJECT
    
public:
    /** @brief Sets up the UI and sets the spline to @param spline. */
    BezierSplineWidget(const QString &spline, QWidget* parent = 0);

    /** @brief Returns the current spline. */
    QString spline();

    /** The curvemodes refer to the usage of the spline.
     * As this widget is currently only used for frei0r.curves the modes are the channels this filter accepts. */
    enum CurveModes { ModeRed, ModeGreen, ModeBlue, ModeAlpha, ModeLuma, ModeRGB, ModeHue, ModeSaturation };

    /** @brief Sets the mode to @param mode and updates the editors background pixmap if necessary. */
    void setMode(CurveModes mode);

private slots:
    /** @brief Sets the spinboxes for modifing the selected point to @param p. */
    void slotUpdatePointEntries(const BPoint &p);

    /** @brief Updates the editor and thus the spline if the current point's p was modified using the spinboxes. */
    void slotUpdatePointP();
    /** @brief Updates the editor and thus the spline if the current point's h1 was modified using the spinboxes. */
    void slotUpdatePointH1();
    /** @brief Updates the editor and thus the spline if the current point's h2 was modified using the spinboxes. */
    void slotUpdatePointH2();

    /** @brief Increases the number of lines in the editor's grid. If there are already 8 lines the number is set to 0. */
    void slotGridChange();
    /** @brief Turns showing the background pixmap in the editor on/off. */
    void slotShowPixmap(bool show = true);
    /** @brief Resets the current spline. */
    void slotResetSpline();
    /** @brief Linkes the handles. This will always make them stay in one line through p. */
    void slotSetHandlesLinked(bool linked);

private:
    Ui::BezierSpline_UI m_ui;
    BezierSplineEditor m_edit;
    CurveModes m_mode;
    bool m_showPixmap;

signals:
    void modified();
};

#endif
