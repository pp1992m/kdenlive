/***************************************************************************
                         krender.h  -  description
                            -------------------
   begin                : Fri Nov 22 2002
   copyright            : (C) 2002 by Jason Wood (jasonwood@blueyonder.co.uk)
   copyright            : (C) 2010 by Jean-Baptiste Mardelle (jb@kdenlive.org)

***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/**
 * @class Render
 * @brief Client side of the interface to a renderer.
 *
 * From Kdenlive's point of view, you treat the Render object as the renderer,
 * and simply use it as if it was local. Calls are asynchronous - you send a
 * call out, and then receive the return value through the relevant signal that
 * get's emitted once the call completes.
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "gentime.h"
#include "definitions.h"
#include "abstractmonitor.h"
#include "mlt/framework/mlt_types.h"

#include <kurl.h>

#include <qdom.h>
#include <qstring.h>
#include <qmap.h>
#include <QList>
#include <QEvent>
#include <QMutex>

class QTimer;
class QPixmap;

namespace Mlt
{
class Consumer;
class Playlist;
class Tractor;
class Transition;
class Frame;
class Field;
class Producer;
class Filter;
class Profile;
class Service;
};

class MltErrorEvent : public QEvent
{
public:
    MltErrorEvent(QString message) : QEvent(QEvent::User), m_message(message) {}
    QString message() const {
        return m_message;
    }

private:
    QString m_message;
};


class Render: public AbstractRender
{
Q_OBJECT public:

    enum FailStates { OK = 0,
                      APP_NOEXIST
                    };
    /** @brief Build a MLT Renderer
     *  @param rendererName A unique identifier for this renderer
     *  @param winid The parent widget identifier (required for SDL display). Set to 0 for OpenGL rendering
     *  @param profile The MLT profile used for the renderer (default one will be used if empty). */
    Render(const QString & rendererName, int winid, QString profile = QString(), QWidget *parent = 0);

    /** @brief Destroy the MLT Renderer. */
    virtual ~Render();

    /** @brief Seeks the renderer clip to the given time. */
    void seek(GenTime time);
    void seek(int time);
    void seekToFrameDiff(int diff);
    int m_isBlocked;

    QPixmap getImageThumbnail(KUrl url, int width, int height);

    /** @brief Sets the current MLT producer playlist.
     * @param list The xml describing the playlist
     * @param position (optional) time to seek to */
    int setSceneList(QDomDocument list, int position = 0);

    /** @brief Sets the current MLT producer playlist.
     * @param list new playlist
     * @param position (optional) time to seek to
     * @return 0 when it has success, different from 0 otherwise
     *
     * Creates the producer from the text playlist. */
    int setSceneList(QString playlist, int position = 0);
    int setProducer(Mlt::Producer *producer, int position);

    /** @brief Get the current MLT producer playlist.
     * @return A string describing the playlist */
    const QString sceneList();
    bool saveSceneList(QString path, QDomElement kdenliveData = QDomElement());

    /** @brief Tells the renderer to play the scene at the specified speed,
     * @param speed speed to play the scene to
     *
     * The speed is relative to normal playback, e.g. 1.0 is normal speed, 0.0
     * is paused, -1.0 means play backwards. It does not specify start/stop */
    void play(double speed);
    void switchPlay(bool play);
    void pause();

    /** @brief Stops playing.
     * @param startTime time to seek to */
    void stop(const GenTime & startTime);
    int volume() const;

    QImage extractFrame(int frame_position, QString path = QString(), int width = -1, int height = -1);

    /** @brief Plays the scene starting from a specific time.
     * @param startTime time to start playing the scene from */
    void play(const GenTime & startTime);
    void playZone(const GenTime & startTime, const GenTime & stopTime);
    void loopZone(const GenTime & startTime, const GenTime & stopTime);

    void saveZone(KUrl url, QString desc, QPoint zone);

    /** @brief Returns the name of the renderer. */
    const QString & rendererName() const;

    /** @brief Returns the speed at which the renderer is currently playing.
     *
     * It returns 0.0 when the renderer is not playing anything. */
    double playSpeed();

    /** @brief Returns the current seek position of the renderer. */
    GenTime seekPosition() const;
    int seekFramePosition() const;

    void emitFrameUpdated(Mlt::Frame&);
    void emitFrameNumber(double position);
    void emitConsumerStopped();

    /** @brief Returns the aspect ratio of the consumer. */
    double consumerRatio() const;

    void doRefresh();

    /** @brief Saves current producer frame as an image. */
    void exportCurrentFrame(KUrl url, bool notify);

    /** @brief Turns on or off on screen display. */
    void refreshDisplay();
    /** @brief Change the Mlt PROFILE
     * @param profileName The MLT profile name
     * @param dropSceneList If true, the current playlist will be deleted
     * . */
    int resetProfile(const QString profileName, bool dropSceneList = false);
    double fps() const;

    /** @brief Returns the width of a frame for this profile. */
    int frameRenderWidth() const;
    /** @brief Returns the display width of a frame for this profile. */
    int renderWidth() const;
    /** @brief Returns the height of a frame for this profile. */
    int renderHeight() const;

    /** @brief Returns display aspect ratio. */
    double dar() const;
    /** @brief Returns sample aspect ratio. */
    double sar() const;

    /*
     * Playlist manipulation.
     */
    Mlt::Producer *checkSlowMotionProducer(Mlt::Producer *prod, QDomElement element);
    int mltInsertClip(ItemInfo info, QDomElement element, Mlt::Producer *prod, bool overwrite = false, bool push = false);
    bool mltUpdateClip(ItemInfo info, QDomElement element, Mlt::Producer *prod);
    void mltCutClip(int track, GenTime position);
    void mltInsertSpace(QMap <int, int> trackClipStartList, QMap <int, int> trackTransitionStartList, int track, const GenTime duration, const GenTime timeOffset);
    int mltGetSpaceLength(const GenTime pos, int track, bool fromBlankStart);

    /** @brief Returns the duration/length of @param track as reported by the track producer. */
    int mltTrackDuration(int track);

    bool mltResizeClipEnd(ItemInfo info, GenTime clipDuration);
    bool mltResizeClipStart(ItemInfo info, GenTime diff);
    bool mltResizeClipCrop(ItemInfo info, GenTime diff);
    bool mltMoveClip(int startTrack, int endTrack, GenTime pos, GenTime moveStart, Mlt::Producer *prod, bool overwrite = false, bool insert = false);
    bool mltMoveClip(int startTrack, int endTrack, int pos, int moveStart, Mlt::Producer *prod, bool overwrite = false, bool insert = false);
    bool mltRemoveClip(int track, GenTime position);

    /** @brief Deletes an effect from a clip in MLT's playlist. */
    bool mltRemoveEffect(int track, GenTime position, QString index, bool updateIndex, bool doRefresh = true);
    bool mltRemoveTrackEffect(int track, QString index, bool updateIndex);

    /** @brief Adds an effect to a clip in MLT's playlist. */
    bool mltAddEffect(int track, GenTime position, EffectsParameterList params, bool doRefresh = true);
    bool mltAddEffect(Mlt::Service service, EffectsParameterList params, int duration, bool doRefresh);
    bool mltAddTrackEffect(int track, EffectsParameterList params);

    /** @brief Edits an effect parameters in MLT's playlist. */
    bool mltEditEffect(int track, GenTime position, EffectsParameterList params);
    bool mltEditTrackEffect(int track, EffectsParameterList params);

    /** @brief Updates the "kdenlive_ix" (index) value of an effect. */
    void mltUpdateEffectPosition(int track, GenTime position, int oldPos, int newPos);

    /** @brief Changes the order of effects in MLT's playlist.
     *
     * It switches effects from oldPos and newPos, updating the "kdenlive_ix"
     * (index) value. */
    void mltMoveEffect(int track, GenTime position, int oldPos, int newPos);
    void mltMoveTrackEffect(int track, int oldPos, int newPos);

    /** @brief Enables/disables audio/video in a track. */
    void mltChangeTrackState(int track, bool mute, bool blind);
    bool mltMoveTransition(QString type, int startTrack,  int newTrack, int newTransitionTrack, GenTime oldIn, GenTime oldOut, GenTime newIn, GenTime newOut);
    bool mltAddTransition(QString tag, int a_track, int b_track, GenTime in, GenTime out, QDomElement xml, bool refresh = true);
    void mltDeleteTransition(QString tag, int a_track, int b_track, GenTime in, GenTime out, QDomElement xml, bool refresh = true);
    void mltUpdateTransition(QString oldTag, QString tag, int a_track, int b_track, GenTime in, GenTime out, QDomElement xml, bool force = false);
    void mltUpdateTransitionParams(QString type, int a_track, int b_track, GenTime in, GenTime out, QDomElement xml);
    void mltAddClipTransparency(ItemInfo info, int transitiontrack, int id);
    void mltMoveTransparency(int startTime, int endTime, int startTrack, int endTrack, int id);
    void mltDeleteTransparency(int pos, int track, int id);
    void mltResizeTransparency(int oldStart, int newStart, int newEnd, int track, int id);
    void mltInsertTrack(int ix, bool videoTrack);
    void mltDeleteTrack(int ix);
    bool mltUpdateClipProducer(int track, int pos, Mlt::Producer *prod);
    void mltPlantTransition(Mlt::Field *field, Mlt::Transition &tr, int a_track, int b_track);
    Mlt::Producer *invalidProducer(const QString &id);

    /** @brief Changes the speed of a clip in MLT's playlist.
     *
     * It creates a new "framebuffer" producer, which must have its "resource"
     * property set to "video.mpg?0.6", where "video.mpg" is the path to the
     * clip and "0.6" is the speed in percentage. The newly created producer
     * will have its "id" property set to "slowmotion:parentid:speed", where
     * "parentid" is the id of the original clip in the ClipManager list and
     * "speed" is the current speed. */
    int mltChangeClipSpeed(ItemInfo info, ItemInfo speedIndependantInfo, double speed, double oldspeed, int strobe, Mlt::Producer *prod);

    const QList <Mlt::Producer *> producersList();
    void updatePreviewSettings();
    void setDropFrames(bool show);
    QString updateSceneListFps(double current_fps, double new_fps, QString scene);
    void showFrame(Mlt::Frame&);

    void showAudio(Mlt::Frame&);
    /** @brief This property is used to decide if the renderer should send audio data for monitoring. */
    bool analyseAudio;
    
    QList <int> checkTrackSequence(int);
    void sendFrameUpdate();

    /** @brief Returns a pointer to the main producer. */
    Mlt::Producer *getProducer();

private:

    /** @brief The name of this renderer.
     *
     * Useful to identify the renderers by what they do - e.g. background
     * rendering, workspace monitor, etc. */
    QString m_name;
    Mlt::Consumer * m_mltConsumer;
    Mlt::Producer * m_mltProducer;
    Mlt::Profile *m_mltProfile;
    double m_framePosition;
    double m_fps;
    bool m_externalConsumer;

    /** @brief True if we are playing a zone.
     *
     * It's determined by the "in" and "out" properties being temporarily
     * changed. */
    bool m_isZoneMode;
    bool m_isLoopMode;
    GenTime m_loopStart;
    int m_originalOut;

    /** @brief True when the monitor is in split view. */
    bool m_isSplitView;

    Mlt::Producer *m_blackClip;
    QString m_activeProfile;

    QTimer *m_osdTimer;
    QMutex m_mutex;

    /** @brief A human-readable description of this renderer. */
    int m_winid;

    void closeMlt();
    void mltCheckLength(Mlt::Tractor *tractor);
    void mltPasteEffects(Mlt::Producer *source, Mlt::Producer *dest);
    QMap<QString, QString> mltGetTransitionParamsFromXml(QDomElement xml);
    QMap<QString, Mlt::Producer *> m_slowmotionProducers;

    /** @brief Build the MLT Consumer object with initial settings.
     *  @param profileName The MLT profile to use for the consumer */
    void buildConsumer(const QString profileName);
    void resetZoneMode();
    void fillSlowMotionProducers();
    /** @brief Get the track number of the lowest audible (non muted) audio track
     *  @param return The track number */
    int getLowestNonMutedAudioTrack(Mlt::Tractor tractor);

    /** @brief Make sure our audio mixing transitions are applied to the lowest track */
    void fixAudioMixing(Mlt::Tractor tractor);

private slots:

    /** @brief Refreshes the monitor display. */
    void refresh();
    void slotOsdTimeout();
    int connectPlaylist();
    //void initSceneList();

signals:

    /** @brief The renderer received a reply to a getFileProperties request. */
    void replyGetFileProperties(const QString &clipId, Mlt::Producer*, const QMap < QString, QString > &, const QMap < QString, QString > &, bool, bool);

    /** @brief The renderer received a reply to a getImage request. */
    void replyGetImage(const QString &, const QPixmap &);

    /** @brief The renderer stopped, either playing or rendering. */
    void stopped();

    /** @brief The renderer started playing. */
    void playing(double);

    /** @brief The renderer started rendering. */
    void rendering(const GenTime &);

    /** @brief An error occurred within this renderer. */
    void error(const QString &, const QString &);
    void durationChanged(int);
    void rendererPosition(int);
    void rendererStopped(int);
    /** @brief The clip is not valid, should be removed from project. */
    void removeInvalidClip(const QString &, bool replaceProducer);
    /** @brief The proxy is not valid, should be deleted.
     *  @param id The original clip's id
     *  @param durationError Should be set to true if the proxy failed because it has not same length as original clip
     */
    void removeInvalidProxy(const QString &id, bool durationError);
    void refreshDocumentProducers(bool displayRatioChanged, bool fpsChanged);
    
    /** @brief If we will delete the producer, make sure to pause the monitor */
    void blockClipMonitor(const QString);

    /** @brief A frame's image has to be shown.
     *
     * Used in Mac OS X. */
    void showImageSignal(QImage);
    void showAudioSignal(const QByteArray);

public slots:

    /** @brief Starts the consumer. */
    void start();

    /** @brief Stops the consumer. */
    void stop();
    int getLength();

    /** @brief Checks if the file is readable by MLT. */
    bool isValid(KUrl url);

    /** @brief Requests the file properties for the specified URL.
        @param xml The xml parameters for the clip
        @param clipId The clip Id string
        @param imageHeight The height (in pixels) of the returned thumbnail (height of a treewidgetitem in projectlist)
        @param replaceProducer If true, the MLT producer will be recreated
        @param selectClip If true, clip item will be selected in project view
     * Upon return, the result will be emitted via replyGetFileProperties().
     * Wraps the VEML command of the same name. */
    void getFileProperties(const QDomElement xml, const QString &clipId, int imageHeight, bool replaceProducer = true, bool selectClip = false);

    void exportFileToFirewire(QString srcFileName, int port, GenTime startTime, GenTime endTime);
    void mltSavePlaylist();
    void slotSplitView(bool doit);
    void slotSwitchFullscreen();
    void slotSetVolume(int volume);
    void seekToFrame(int pos);
};

#endif
