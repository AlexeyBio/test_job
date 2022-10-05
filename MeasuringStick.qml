import QtQuick 2.0
import "Helper.js" as Helper

Item{
    id: root
    property var groundZero;
    property var beginMeasuredData;
    property var endMeasuredData;
    property var rankingsGraduation: [];
    property int minimalDistantionBetweenGraduation: 15;//px
    readonly property var measuredData: Helper.measuredData(root);
    readonly property var indexBaseRank: Helper.indexBaseRank(root);

    Repeater{
        id: repeater
        model: Helper.makeModel(root);
        Item{
            property int measuredValue: (modelData - root.groundZero)
                               % root.rankingsGraduation[indexBaseRank + 1];
            x: Helper.measuredValueToViewPoint(modelData, root);
            anchors.top: parent.top;
            anchors.bottom: parent.bottom;
            Rectangle{
                id: graduation
                height: parent.height * 0.5 * ( 0.5 + 0.5 * (measuredValue === 0))
                width: 1;
                color: "black"
            }
            Text{
                anchors.top: graduation.bottom
                anchors.horizontalCenter: parent.left
                visible: measuredValue === 0;
                height: parent.height * 0.5
                text: (new Date(modelData)).toLocaleTimeString(Qt.locale(), "h:mm");
            }
        }
    }
}
