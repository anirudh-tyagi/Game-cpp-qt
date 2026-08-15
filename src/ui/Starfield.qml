import QtQuick

//Static backdrop for the night sky. Purely decorative, it holds no game state
//and never moves, so it costs one batch of small rectangles and nothing else.
Item {
    id: field

    //seeded once at creation, otherwise a resize would reshuffle the whole sky
    property var stars: []

    Component.onCompleted: {
        let seeded = []
        for (let i = 0; i < Theme.starCount; i++) {
            seeded.push({
                //stored as fractions so the sky reflows with the window
                "fx": Math.random(),
                "fy": Math.random(),
                "size": 1 + (Math.random() * 1.6),
                "dim": 0.15 + (Math.random() * 0.45),
                //a slow, uneven twinkle, nothing in step with anything else
                "period": 2200 + Math.floor(Math.random() * 3400)
            })
        }
        field.stars = seeded
    }

    //a faint lift towards the horizon so the bottom of the screen, where the
    //ship sits, is not a flat void
    Rectangle {
        anchors.fill: parent

        gradient: Gradient {
            GradientStop { position: 0.0; color: Theme.background }
            GradientStop { position: 1.0; color: Theme.backgroundGlow }
        }
    }

    Repeater {
        model: field.stars

        delegate: Rectangle {
            required property var modelData

            x: modelData.fx * field.width
            y: modelData.fy * field.height
            width: modelData.size
            height: modelData.size
            radius: width / 2
            color: Theme.starColor
            opacity: modelData.dim

            SequentialAnimation on opacity {
                loops: Animation.Infinite

                NumberAnimation {
                    to: modelData.dim * 0.35
                    duration: modelData.period
                    easing.type: Easing.InOutSine
                }
                NumberAnimation {
                    to: modelData.dim
                    duration: modelData.period
                    easing.type: Easing.InOutSine
                }
            }
        }
    }
}
