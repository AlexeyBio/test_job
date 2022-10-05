import QtQuick 2.0

Item {
    id: root;
    property var model;
    property var groundZero;
    property var beginMeasuredData;
    property var endMeasuredData;
    readonly property var groupedOnPart: labelView.groupedOnPart
    signal hoveredBookmark(var name)
    signal hoveredGroupBookmark(var list)

    MeasuringStick{
        id: measuringStick
        groundZero: root.groundZero;
        beginMeasuredData: root.beginMeasuredData;
        endMeasuredData: root.endMeasuredData;
        rankingsGraduation: [
            1000,                   // s
            30 * 1000,
            60 * 1000,              // m
            30 * 60 * 1000,
            60 * 60 * 1000,         // h
            12 * 60 * 60 * 1000,
            24 * 60 * 60 * 1000     // d
            //TODO etc
        ];
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: root.height * 0.5
    }
    LabelView{
        id: labelView
        groundZero: root.groundZero;
        beginMeasuredData: root.beginMeasuredData;
        endMeasuredData: root.endMeasuredData;
        model: root.model;
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: measuringStick.bottom
        anchors.bottom: parent.bottom
        onHoveredBookmark: root.hoveredBookmark(name);
        onHoveredGroupBookmark: root.hoveredGroupBookmark(list);
    }
}
