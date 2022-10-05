import QtQuick 2.0
import "Helper.js" as Helper

Item{
    id: root
    property var groupedOnPart: {
        var point = delegateSize;
        if (point > 0 && root.measuredData){
            return Math.round(point * root.measuredData / root.width);
        }
        return 0;
    }
    property var groundZero;
    property var beginMeasuredData;
    property var endMeasuredData;
    property alias model: repeater.model;
    readonly property var measuredData: Helper.measuredData(root);
    property int delegateSize: 100 //px

    signal hoveredGroupBookmark(var list);
    signal hoveredBookmark(var name);
    Repeater{
        id: repeater
        Rectangle{
            id: label
            property var measuredValue: (new Date(time)).getTime();
            x: Helper.measuredValueToViewPoint(measuredValue, root);
            color: isGroup? "green" : "blue"
            border.width: 1;
            border.color: Qt.lighter(color);
            anchors.top: parent.top;
            anchors.bottom: parent.bottom;
            width: delegateSize;
            radius: 4;
            Text{
                anchors.centerIn: parent;
                text: name;
                color: "white";
            }
            MouseArea{
                anchors.fill: parent;
                hoverEnabled: true;
                onEntered:  {
                    if (isGroup){
                        hoveredGroupBookmark(groupModel)
                    } else {
                        hoveredBookmark(name);
                    }
                }
            }
        }
    }
}
