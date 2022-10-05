.pragma library
//INFO this functions fixed bugs a calculating in binding

function indexBaseRank(measuringStick) {
    var rank = measuringStick.rankingsGraduation,
        distantionGraduation = measuringStick.minimalDistantionBetweenGraduation,
        measuredData = measuringStick.measuredData,
        viewWidth = measuringStick.width;
    var index = 0;
    if (rank.legnth === 0 || measuredData <= 0)
        return -1;
    var minGraduationOnLine = viewWidth / distantionGraduation;
    while (measuredData / rank[index] > minGraduationOnLine
           && index < rank.length) ++index;
    return index;
}

function measuredData(measuringStick){
    if (!measuringStick.endMeasuredData && !measuringStick.beginMeasuredData)
        return 0;
    if (isNaN(measuringStick.endMeasuredData)
            || isNaN(measuringStick.beginMeasuredData))
        return 0;
    var diff = measuringStick.endMeasuredData - measuringStick.beginMeasuredData;
    return diff;
}

function makeModel(measuringStick){
    var rank = measuringStick.rankingsGraduation,
        measuredData = measuringStick.measuredData,
        it = measuringStick.beginMeasuredData,
        end = measuringStick.endMeasuredData;

    if (measuringStick.indexBaseRank < 0)
        return [];
    var datatimes = [],
        step = rank[measuringStick.indexBaseRank];
    for (; it <= end; it += step) datatimes.push(it);
    return datatimes;
}

function measuredValueToViewPoint(value, measuringStick){
    return (value - measuringStick.beginMeasuredData)
            * measuringStick.width / measuringStick.measuredData;
}


function measuredViewPointToValue(point, measuringStick){

    if (point > 0 && measuringStick.measuredData){
        return point * measuringStick.measuredData / measuringStick.width;
    }
    return 0;
}
