import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

// ServerListToggle
Image {
    Layout.preferredHeight: 8
    Layout.preferredWidth: 8

    fillMode: Image.PreserveAspectFit
    smooth: true
    source: "../resources/toggle-down.svg"
    transformOrigin: Image.Center

    states: State {
        name: "rotated"
        when: serverCountry.cityListVisible
        PropertyChanges {
            target: serverListToggle
            rotation: 90
        }
    }

    transitions: [
        Transition {
            to: "rotated"
            RotationAnimation {
                properties: "rotation"
                duration: 200
                direction: RotationAnimation.Clockwise
                easing.type: Easing.InOutQuad
            }
        },

        Transition {
            to: ""
            RotationAnimation {
                properties: "rotation"
                duration: 200
                direction: RotationAnimation.Counterclockwise
                easing.type: Easing.InOutQuad
            }
        }
    ]
}