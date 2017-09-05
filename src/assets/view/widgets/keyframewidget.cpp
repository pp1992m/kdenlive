/***************************************************************************
 *   Copyright (C) 2011 by Till Theato (root@ttill.de)                     *
 *   Copyright (C) 2017 by Nicolas Carion                                  *
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

#include "keyframewidget.hpp"
#include "assets/keyframes/view/keyframeview.hpp"
#include "assets/keyframes/model/keyframemodellist.hpp"
#include "timecodedisplay.h"
#include "utils/KoIconUtils.h"
#include "core.h"

#include <QGridLayout>
#include <QToolButton>

#include <klocalizedstring.h>

KeyframeWidget::KeyframeWidget(std::shared_ptr<AssetParameterModel> model, QModelIndex index, double init_value, const Timecode &t, int duration, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_index(index)
    , m_keyframes(new KeyframeModelList(init_value, model, index, pCore->undoStack()))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto *l = new QGridLayout(this);

    m_keyframeview = new KeyframeView(m_keyframes, this);
    m_keyframeview->setDuration(duration);

    m_buttonAddDelete = new QToolButton(this);
    m_buttonAddDelete->setAutoRaise(true);
    m_buttonAddDelete->setIcon(KoIconUtils::themedIcon(QStringLiteral("list-add")));
    m_buttonAddDelete->setToolTip(i18n("Add keyframe"));

    m_buttonPrevious = new QToolButton(this);
    m_buttonPrevious->setAutoRaise(true);
    m_buttonPrevious->setIcon(KoIconUtils::themedIcon(QStringLiteral("media-skip-backward")));
    m_buttonPrevious->setToolTip(i18n("Go to previous keyframe"));

    m_buttonNext = new QToolButton(this);
    m_buttonNext->setAutoRaise(true);
    m_buttonNext->setIcon(KoIconUtils::themedIcon(QStringLiteral("media-skip-forward")));
    m_buttonNext->setToolTip(i18n("Go to next keyframe"));

    m_time = new TimecodeDisplay(t, this);
    m_time->setRange(0, duration);

    l->addWidget(m_keyframeview, 0, 0, 1, -1);
    l->addWidget(m_buttonPrevious, 1, 0);
    l->addWidget(m_buttonAddDelete, 1, 1);
    l->addWidget(m_buttonNext, 1, 2);
    l->addWidget(m_time, 1, 3, Qt::AlignRight);

    connect(m_time, &TimecodeDisplay::timeCodeEditingFinished, [&](){slotSetPosition(-1, true);});
    connect(m_keyframeview, &KeyframeView::positionChanged, [&](int p){slotSetPosition(p, true);});
    connect(m_keyframeview, &KeyframeView::atKeyframe, this, &KeyframeWidget::slotAtKeyframe);

    connect(m_buttonAddDelete, &QAbstractButton::pressed, m_keyframeview, &KeyframeView::slotAddRemove);
    connect(m_buttonPrevious, &QAbstractButton::pressed, m_keyframeview, &KeyframeView::slotGoToPrev);
    connect(m_buttonNext, &QAbstractButton::pressed, m_keyframeview, &KeyframeView::slotGoToNext);

}

KeyframeWidget::~KeyframeWidget()
{
    delete m_keyframeview;
    delete m_buttonAddDelete;
    delete m_buttonPrevious;
    delete m_buttonNext;
    delete m_time;
}

void KeyframeWidget::slotSetPosition(int pos, bool update)
{
    if (pos < 0) {
        pos = m_time->getValue();
        m_keyframeview->slotSetPosition(pos);
    } else {
        m_time->setValue(pos);
        m_keyframeview->slotSetPosition(pos);
    }

    if (update) {
        emit positionChanged(pos);
    }
}

int KeyframeWidget::getPosition() const
{
    return m_time->getValue();
}


void KeyframeWidget::addKeyframe(int pos)
{
    blockSignals(true);
    m_keyframeview->slotAddKeyframe(pos);
    blockSignals(false);
    setEnabled(true);
}

void KeyframeWidget::updateTimecodeFormat()
{
    m_time->slotUpdateTimeCodeFormat();
}

void KeyframeWidget::slotAtKeyframe(bool atKeyframe)
{
    if (atKeyframe) {
        m_buttonAddDelete->setIcon(KoIconUtils::themedIcon(QStringLiteral("list-remove")));
        m_buttonAddDelete->setToolTip(i18n("Delete keyframe"));
    } else {
        m_buttonAddDelete->setIcon(KoIconUtils::themedIcon(QStringLiteral("list-add")));
        m_buttonAddDelete->setToolTip(i18n("Add keyframe"));
    }
}