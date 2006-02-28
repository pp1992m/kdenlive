/***************************************************************************
                          transition.h  -  description
                             -------------------
    begin                : Tue Jan 24 2006
    copyright            : (C) 2006 by Jean-Baptiste Mardelle
    email                : jb@ader.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TRANSITION_H
#define TRANSITION_H

#include <qstring.h>
#include <qptrlist.h>
#include <qdom.h>

#include "gentime.h"



/**Describes a Transition, with a name, parameters keyframes, etc.
  *@author Jean-Baptiste Mardelle
  */

class DocClipRef;

class Transition {
  public:
    Transition(const DocClipRef * clipa, const DocClipRef * clipb);
    Transition(const DocClipRef * clipa);
    ~Transition();


    GenTime transitionStartTime();
    GenTime transitionEndTime();
    int transitionStartTrack();
    int transitionEndTrack();
    Transition *clone();
    bool hasClip(const DocClipRef * clip);
    void resizeTransitionEnd(GenTime time);
    void resizeTransitionStart(GenTime time);
    void moveTransition(GenTime time);
    bool invertTransition();
    
  private:
    
    GenTime m_transitionStart;
    GenTime m_transitionDuration;
    
    /** Should the transition be reversed */
    bool m_invertTransition;
    
    bool m_singleClip;
    
    /** The track to which the transition is attached*/
    int m_track;
    
    /** The clip to which the transition is attached */
    const DocClipRef *m_referenceClip;
    
    /** The 2nd clip to which the transition is attached */
    const DocClipRef *m_secondClip;
};

#endif
