/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.5
import QtGraphicalEffects 1.15
import QtQuick.Layouts 1.15
import "../themes/themes.js" as Theme

//VPNHeadline
Text {
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    wrapMode: Text.WordWrap
    width: Theme.maxTextWidth
    color: Theme.fontColorDark
    font.family: Theme.fontFamily
    font.pixelSize: 22
    lineHeightMode: Text.FixedHeight
    lineHeight: 32
    Layout.alignment: Qt.AlignHCenter
}
