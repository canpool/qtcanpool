/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

import QtQuick 2.0

Rectangle
{
    property url imgSource
    property color buttonColor
    property color buttonColorPressed
    property bool buttonPressed: false

    color: "transparent"

    function hoverEnter()
    {
        if (!buttonPressed)
        {
            rect.color = buttonColor
            opacityAnimatorLeave.stop()
            opacityAnimatorEnter.start()
        }
        else
        {
            rect.color = buttonColorPressed
            opacityAnimatorEnter.stop()
            opacityAnimatorLeave.stop()
            rect.opacity = 1.0
        }
    }

    function hoverLeave()
    {
        if (!buttonPressed)
        {
            rect.color = buttonColor
            opacityAnimatorEnter.stop()
            opacityAnimatorLeave.start()
        }
        else
        {
            opacityAnimatorEnter.stop()
            opacityAnimatorLeave.stop()
            rect.opacity = 0.0
        }
    }

    function buttonPress()
    {
        buttonPressed = true

        rect.color = buttonColorPressed
        opacityAnimatorEnter.stop()
        opacityAnimatorLeave.stop()
    }

    function buttonRelease()
    {
        buttonPressed = false

        opacityAnimatorEnter.stop()
        opacityAnimatorLeave.stop()
        rect.opacity = 0.0
    }

    Rectangle
    {
        id: rect
        anchors.centerIn: parent
        width: 20
        height: 20
        radius: 20
        opacity: 0.0

        NumberAnimation on opacity
        {
            id: opacityAnimatorEnter
            to: 1.0
            duration: 100
            running: false
        }

        NumberAnimation on opacity
        {
            id: opacityAnimatorLeave
            to: 0.0
            duration: 100
            running: false
        }
    }

    Image
    {
        id: icon
        anchors.centerIn: rect
        width: 10
        height: 10
        smooth: true
        source: imgSource
    }
}
