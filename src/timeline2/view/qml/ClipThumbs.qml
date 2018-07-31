import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQml.Models 2.2
import com.enums 1.0


Row {
    id: thumbRow
    anchors.fill: parent
    visible: timeline.showThumbnails && mltService != 'color' && !isAudio
    opacity: parentTrack.isAudio || parentTrack.isHidden || clipStatus == ClipState.Disabled ? 0.2 : 1
    property int thumbWidth: container.height * 16.0/9.0
    property int scrollStart: Math.max(0, scrollView.flickableItem.contentX / timeline.scaleFactor - clipRoot.modelStart)
    property int scrollEnd: scrollStart + scrollView.viewport.width / timeline.scaleFactor
    Repeater {
        id: thumbRepeater
        // switching the model allows to have different view modes:
        // 2: will display start / end thumbs
        // container.width / thumbRow.thumbWidth will display all frames showThumbnails
        // 0: will disable thumbnails
        model: parentTrack.thumbsFormat == 0 ? 2 : parentTrack.thumbsFormat == 1 ? container.width / thumbRow.thumbWidth : 0
        property int startFrame: clipRoot.inPoint
        property int endFrame: clipRoot.outPoint
        property real imageWidth: container.width / thumbRepeater.count
        Image {
            width: thumbRepeater.imageWidth
            height: container.height
            fillMode: Image.PreserveAspectFit
            asynchronous: true
            cache: false
            property int currentFrame: Math.floor(clipRoot.inPoint + Math.round((index) * width / timeline.scaleFactor)* clipRoot.speed)
            horizontalAlignment: thumbRepeater.count == 2 ? (index == 0 ? Image.AlignLeft : Image.AlignRight) : Image.AlignHCenter
            source: thumbRepeater.count == 2 ? (index == 0 ? clipRoot.baseThumbPath + Math.floor(clipRoot.inPoint * clipRoot.speed) : clipRoot.baseThumbPath + Math.floor(clipRoot.outPoint * clipRoot.speed)) : (currentFrame + width / timeline.scaleFactor < thumbRow.scrollStart || currentFrame > thumbRow.scrollEnd) ? '' : clipRoot.baseThumbPath + currentFrame
        }
    }
}