import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import Test.job 1.0

Window {
    visible: true
    width: 800
    height: 600
    title: qsTr("Timeline")
    Item {
        id: root
        anchors.fill: parent
        BookmarkModel{
            id: bookmarkModel
            Component.onCompleted: {
                generate(new Date(2000, 1, 1),
                         new Date(2000, 1, 2), 1000);
                setFilterRangeDateTime(new Date(2000, 1, 1),
                                       new Date(2000, 1, 2));//TODO scrolling a time shift
            }
        }

        Timer{
            id: timer;
            interval: 50
            onTriggered: {
                bookmarkModel.durationOnPart = timeLine.groupedOnPart;
            }
        }
        Timeline{
            id: timeLine
            anchors.left: parent.left;
            anchors.right: parent.right;
            height: 50;
            model: bookmarkModel;
            groundZero: bookmarkModel.filterMinimumDateTime.getTime();
            beginMeasuredData: bookmarkModel.filterMinimumDateTime.getTime();
            endMeasuredData:  bookmarkModel.filterMaximumDateTime.getTime();
            onGroupedOnPartChanged: timer.restart();
            onHoveredBookmark: textArea.text = name;
            onHoveredGroupBookmark: {
                var text = "";
                for (var i = 0; i < list.length; ++i){
                    text += list[i].name + "\n";
                }
                textArea.text = text;
            }
        }
        TextArea{
            id: textArea;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.top: timeLine.bottom;
            anchors.bottom: parent.bottom;
        }
    }
}
